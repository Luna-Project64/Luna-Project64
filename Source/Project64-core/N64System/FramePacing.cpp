#include "stdafx.h"
#include "FramePacing.h"
#include "HostScheduler.h"

#include <Common/path.h>

#include <condition_variable>
#include <mutex>
#include <thread>

#include <stdarg.h>
#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{

const char * const CostNames[FrameCost_Count] = { "emu", "rsp", "gfx", "audio", "pif", "compile", "state", "wait" };

const uint32_t SummaryFrames = 600;
const size_t QueueLines = 256;
const size_t LineLength = 480;
const long MaxLogSize = 4 * 1024 * 1024;
const int MaxCostDepth = 16;

// A frame is a hitch when it finished its work this far past its deadline...
const int64_t HitchLateUs = 2000;
// ...when the limiter's wait overshot by this much...
const int64_t HitchOvershootUs = 1000;
// ...or when it simply took this much longer than a period, for any reason.
const int64_t HitchSlackPercent = 25;

// Upper bounds, in milliseconds, of the frame length histogram in each SUMMARY
const int HistogramLimits[] = { 17, 18, 20, 25, 33, 50 };
const int HistogramBuckets = sizeof(HistogramLimits) / sizeof(HistogramLimits[0]) + 1;

struct Writer
{
    std::mutex Lock;
    std::condition_variable Wake;
    std::thread Thread;
    bool StopRequested;
    FILE * File;

    // The emulation thread formats into one buffer while the writer thread
    // drains the other.
    char Buffers[2][QueueLines][LineLength];
    int Active;
    size_t Count;
    uint32_t Dropped;
};

struct Recorder
{
    bool Active;
#ifdef _WIN32
    DWORD ThreadId;
#endif
    int64_t Frequency;

    // Cost attribution for the frame in progress
    int64_t LastSwitch;
    int Current;
    int Stack[MaxCostDepth];
    int Depth;
    int64_t Cost[FrameCost_Count];
    int64_t FrameStart;
    int64_t WaitRequested;
    int64_t WaitActual;
    int64_t Late;
    bool Reanchored;
    bool Paused;
    uint32_t FrameIndex;

    // Running summary
    uint32_t SummaryCount;
    int64_t SummaryStart;
    int64_t SumTotal;
    int64_t MaxTotal;
    int64_t SumCost[FrameCost_Count];
    int64_t MaxCost[FrameCost_Count];
    uint32_t Hitches;
    uint32_t StateFrames;
    int64_t MaxLate;
    uint32_t Waits;
    int64_t SumOvershoot;
    int64_t MaxOvershoot;
    uint32_t Histogram[HistogramBuckets];

    uint32_t SessionHitches;
    uint32_t SessionFrames;
};

Writer * g_Writer = nullptr;
Recorder g_Rec = {};

int64_t Ticks(void)
{
#ifdef _WIN32
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return Value.QuadPart;
#else
    return 0;
#endif
}

int64_t UsToTicks(int64_t Us)
{
    return Us * g_Rec.Frequency / 1000000;
}

double Ms(int64_t Ticks)
{
    return (double)Ticks * 1000.0 / (double)g_Rec.Frequency;
}

bool OnRecorderThread(void)
{
#ifdef _WIN32
    return g_Rec.Active && GetCurrentThreadId() == g_Rec.ThreadId;
#else
    return g_Rec.Active;
#endif
}

void QueueLine(const char * Format, ...)
{
    if (g_Writer == nullptr)
    {
        return;
    }

    std::lock_guard<std::mutex> Guard(g_Writer->Lock);
    if (g_Writer->Count >= QueueLines)
    {
        g_Writer->Dropped += 1;
        return;
    }
    va_list Args;
    va_start(Args, Format);
    vsnprintf(g_Writer->Buffers[g_Writer->Active][g_Writer->Count], LineLength, Format, Args);
    va_end(Args);
    g_Writer->Buffers[g_Writer->Active][g_Writer->Count][LineLength - 1] = 0;
    g_Writer->Count += 1;
    if (g_Writer->Count == QueueLines / 2)
    {
        g_Writer->Wake.notify_one();
    }
}

void WriterThread(Writer * Self)
{
#ifdef _WIN32
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
#endif
    for (;;)
    {
        int Drain;
        size_t Count;
        uint32_t Dropped;
        bool Stop;
        {
            std::unique_lock<std::mutex> Guard(Self->Lock);
            Self->Wake.wait_for(Guard, std::chrono::seconds(2), [Self] { return Self->StopRequested || Self->Count >= QueueLines / 2; });
            Drain = Self->Active;
            Count = Self->Count;
            Dropped = Self->Dropped;
            Stop = Self->StopRequested;
            Self->Active ^= 1;
            Self->Count = 0;
            Self->Dropped = 0;
        }

        for (size_t i = 0; i < Count; i++)
        {
            fputs(Self->Buffers[Drain][i], Self->File);
            fputc('\n', Self->File);
        }
        if (Dropped != 0)
        {
            fprintf(Self->File, "(%u lines dropped)\n", Dropped);
        }
        if (Count != 0 || Dropped != 0)
        {
            fflush(Self->File);
        }
        if (Stop)
        {
            break;
        }
    }
}

FILE * OpenLog(void)
{
    CPath LogFile(g_Settings->LoadStringVal(Directory_Log).c_str(), "FramePacing.log");
    if (!LogFile.DirectoryExists())
    {
        LogFile.DirectoryCreate();
    }

    const char * Mode = "a";
    FILE * Existing = fopen(LogFile, "rb");
    if (Existing != nullptr)
    {
        fseek(Existing, 0, SEEK_END);
        if (ftell(Existing) > MaxLogSize)
        {
            Mode = "w";
        }
        fclose(Existing);
    }
    return fopen(LogFile, Mode);
}

void TimeOfDay(char * Buffer, size_t Size)
{
#ifdef _WIN32
    SYSTEMTIME Time;
    GetLocalTime(&Time);
    _snprintf_s(Buffer, Size, _TRUNCATE, "%02d:%02d:%02d.%03d", Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
#else
    snprintf(Buffer, Size, "--:--:--.---");
#endif
}

void ResetSummary(int64_t Now)
{
    g_Rec.SummaryCount = 0;
    g_Rec.SummaryStart = Now;
    g_Rec.SumTotal = 0;
    g_Rec.MaxTotal = 0;
    memset(g_Rec.SumCost, 0, sizeof(g_Rec.SumCost));
    memset(g_Rec.MaxCost, 0, sizeof(g_Rec.MaxCost));
    g_Rec.Hitches = 0;
    g_Rec.StateFrames = 0;
    g_Rec.MaxLate = 0;
    g_Rec.Waits = 0;
    g_Rec.SumOvershoot = 0;
    g_Rec.MaxOvershoot = 0;
    memset(g_Rec.Histogram, 0, sizeof(g_Rec.Histogram));
}

void ResetFrame(int64_t Now)
{
    memset(g_Rec.Cost, 0, sizeof(g_Rec.Cost));
    g_Rec.FrameStart = Now;
    g_Rec.WaitRequested = 0;
    g_Rec.WaitActual = 0;
    g_Rec.Late = 0;
    g_Rec.Reanchored = false;
    g_Rec.Paused = false;
}

void QueueSummary(int64_t Now)
{
    if (g_Rec.SummaryCount == 0)
    {
        return;
    }

    char When[16];
    TimeOfDay(When, sizeof(When));
    uint32_t Count = g_Rec.SummaryCount;
    double Seconds = (double)(Now - g_Rec.SummaryStart) / (double)g_Rec.Frequency;

    char Averages[160] = "", Maximums[160] = "";
    for (int i = 0; i < FrameCost_Count; i++)
    {
        size_t Used = strlen(Averages);
        _snprintf_s(Averages + Used, sizeof(Averages) - Used, _TRUNCATE, "%s%s=%.2f", i ? " " : "", CostNames[i], Ms(g_Rec.SumCost[i] / Count));
        Used = strlen(Maximums);
        _snprintf_s(Maximums + Used, sizeof(Maximums) - Used, _TRUNCATE, "%s%s=%.2f", i ? " " : "", CostNames[i], Ms(g_Rec.MaxCost[i]));
    }

    QueueLine("%s SUMMARY frames=%u fps=%.3f avg=%.3fms max=%.2fms hitches=%u savestates=%u maxLate=%.2fms overshoot avg=%.3fms max=%.3fms"
        " | avg %s | max %s | lengths <17=%u <18=%u <20=%u <25=%u <33=%u <50=%u >=50=%u",
        When, Count, Seconds > 0 ? Count / Seconds : 0.0, Ms(g_Rec.SumTotal / Count), Ms(g_Rec.MaxTotal), g_Rec.Hitches, g_Rec.StateFrames,
        Ms(g_Rec.MaxLate), g_Rec.Waits ? Ms(g_Rec.SumOvershoot / g_Rec.Waits) : 0.0, Ms(g_Rec.MaxOvershoot),
        Averages, Maximums,
        g_Rec.Histogram[0], g_Rec.Histogram[1], g_Rec.Histogram[2], g_Rec.Histogram[3], g_Rec.Histogram[4], g_Rec.Histogram[5], g_Rec.Histogram[6]);
}

} // namespace

static void ShutdownWriter(void)
{
    if (g_Writer == nullptr)
    {
        return;
    }
    {
        std::lock_guard<std::mutex> Guard(g_Writer->Lock);
        g_Writer->StopRequested = true;
    }
    g_Writer->Wake.notify_one();
    if (g_Writer->Thread.joinable())
    {
        g_Writer->Thread.join();
    }
    fclose(g_Writer->File);
    delete g_Writer;
    g_Writer = nullptr;
}

void CFramePacing::Start(void)
{
    // A session that ended without Stop() leaves its writer behind
    g_Rec.Active = false;
    ShutdownWriter();

    if (!g_Settings->LoadBool(Setting_FramePacingLog))
    {
        return;
    }

    FILE * File = OpenLog();
    if (File == nullptr)
    {
        return;
    }

    g_Writer = new Writer();
    g_Writer->StopRequested = false;
    g_Writer->File = File;
    g_Writer->Active = 0;
    g_Writer->Count = 0;
    g_Writer->Dropped = 0;

    g_Rec = Recorder();
#ifdef _WIN32
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    g_Rec.Frequency = Frequency.QuadPart;
    g_Rec.ThreadId = GetCurrentThreadId();
#else
    g_Rec.Frequency = 1000000;
#endif
    int64_t Now = Ticks();
    g_Rec.LastSwitch = Now;
    g_Rec.Current = FrameCost_Emulation;
    g_Rec.FrameStart = 0;
    ResetSummary(Now);

    char When[16];
    TimeOfDay(When, sizeof(When));
    QueueLine("");
    QueueLine("%s ==== session start: %s | scheduling: %s ====", When, g_Settings->LoadStringVal(Game_GameName).c_str(), CHostScheduler::Describe().c_str());
    QueueLine("%s HITCH = frame missed its deadline by >%.1fms, limiter wait overshot by >%.1fms, or frame ran >%d%% long. Costs in ms.",
        When, HitchLateUs / 1000.0, HitchOvershootUs / 1000.0, (int)HitchSlackPercent);

    g_Writer->Thread = std::thread(WriterThread, g_Writer);
    g_Rec.Active = true;
}

void CFramePacing::Stop(void)
{
    if (!OnRecorderThread() || g_Writer == nullptr)
    {
        return;
    }

    int64_t Now = Ticks();
    QueueSummary(Now);
    char When[16];
    TimeOfDay(When, sizeof(When));
    QueueLine("%s ==== session end: %u frames, %u hitches ====", When, g_Rec.SessionFrames, g_Rec.SessionHitches);
    g_Rec.Active = false;
    ShutdownWriter();
}

void CFramePacing::Paused(void)
{
    if (OnRecorderThread())
    {
        g_Rec.Paused = true;
    }
}

void CFramePacing::RecordLimiter(int64_t Requested, int64_t Actual, int64_t Late, bool Reanchored)
{
    if (!OnRecorderThread())
    {
        return;
    }
    g_Rec.WaitRequested = Requested;
    g_Rec.WaitActual = Actual;
    g_Rec.Late = Late;
    g_Rec.Reanchored = Reanchored;
}

bool CFramePacing::Enter(FRAME_COST Cost)
{
    if (!OnRecorderThread() || g_Rec.Depth >= MaxCostDepth)
    {
        return false;
    }
    int64_t Now = Ticks();
    g_Rec.Cost[g_Rec.Current] += Now - g_Rec.LastSwitch;
    g_Rec.Stack[g_Rec.Depth++] = g_Rec.Current;
    g_Rec.Current = Cost;
    g_Rec.LastSwitch = Now;
    return true;
}

void CFramePacing::Leave(void)
{
    if (!g_Rec.Active || g_Rec.Depth == 0)
    {
        return;
    }
    int64_t Now = Ticks();
    g_Rec.Cost[g_Rec.Current] += Now - g_Rec.LastSwitch;
    g_Rec.Current = g_Rec.Stack[--g_Rec.Depth];
    g_Rec.LastSwitch = Now;
}

void CFramePacing::FrameEnd(uint32_t TargetHertz)
{
    if (!OnRecorderThread())
    {
        return;
    }

    int64_t Now = Ticks();
    g_Rec.Cost[g_Rec.Current] += Now - g_Rec.LastSwitch;
    g_Rec.LastSwitch = Now;

    if (g_Rec.FrameStart == 0)
    {
        ResetFrame(Now);
        return;
    }

    int64_t Total = Now - g_Rec.FrameStart;
    g_Rec.FrameIndex += 1;
    if (g_Rec.Paused)
    {
        // Wall time across a pause says nothing about pacing
        ResetFrame(Now);
        return;
    }

    if (TargetHertz == 0)
    {
        TargetHertz = 60;
    }
    int64_t Period = g_Rec.Frequency / TargetHertz;
    int64_t Overshoot = g_Rec.WaitRequested > 0 ? g_Rec.WaitActual - g_Rec.WaitRequested : 0;
    bool SavestateFrame = g_Rec.Cost[FrameCost_State] > 0;
    bool Hitch = g_Rec.Late > UsToTicks(HitchLateUs) ||
        Overshoot > UsToTicks(HitchOvershootUs) ||
        Total > Period + Period * HitchSlackPercent / 100;

    g_Rec.SessionFrames += 1;
    g_Rec.SummaryCount += 1;
    g_Rec.SumTotal += Total;
    if (Total > g_Rec.MaxTotal)
    {
        g_Rec.MaxTotal = Total;
    }
    for (int i = 0; i < FrameCost_Count; i++)
    {
        g_Rec.SumCost[i] += g_Rec.Cost[i];
        if (g_Rec.Cost[i] > g_Rec.MaxCost[i])
        {
            g_Rec.MaxCost[i] = g_Rec.Cost[i];
        }
    }
    if (g_Rec.Late > g_Rec.MaxLate)
    {
        g_Rec.MaxLate = g_Rec.Late;
    }
    if (g_Rec.WaitRequested > 0)
    {
        g_Rec.Waits += 1;
        g_Rec.SumOvershoot += Overshoot > 0 ? Overshoot : 0;
        if (Overshoot > g_Rec.MaxOvershoot)
        {
            g_Rec.MaxOvershoot = Overshoot;
        }
    }
    int Bucket = 0;
    while (Bucket < HistogramBuckets - 1 && Ms(Total) >= HistogramLimits[Bucket])
    {
        Bucket += 1;
    }
    g_Rec.Histogram[Bucket] += 1;

    if (Hitch)
    {
        if (SavestateFrame)
        {
            g_Rec.StateFrames += 1;
        }
        else
        {
            g_Rec.Hitches += 1;
            g_Rec.SessionHitches += 1;
        }

        char When[16];
        TimeOfDay(When, sizeof(When));
        QueueLine("%s %s #%u total=%.2fms late=%.2fms over=%.2fms | emu=%.2f rsp=%.2f gfx=%.2f audio=%.2f pif=%.2f compile=%.2f state=%.2f wait=%.2f%s",
            When, SavestateFrame ? "STATE" : "HITCH", g_Rec.FrameIndex, Ms(Total), Ms(g_Rec.Late), Ms(Overshoot > 0 ? Overshoot : 0),
            Ms(g_Rec.Cost[FrameCost_Emulation]), Ms(g_Rec.Cost[FrameCost_Rsp]), Ms(g_Rec.Cost[FrameCost_Gfx]), Ms(g_Rec.Cost[FrameCost_Audio]),
            Ms(g_Rec.Cost[FrameCost_Pif]), Ms(g_Rec.Cost[FrameCost_Compile]), Ms(g_Rec.Cost[FrameCost_State]), Ms(g_Rec.Cost[FrameCost_Wait]),
            g_Rec.Reanchored ? " [schedule restarted]" : "");
    }

    if (g_Rec.SummaryCount >= SummaryFrames)
    {
        QueueSummary(Now);
        ResetSummary(Now);
    }
    ResetFrame(Now);
}
