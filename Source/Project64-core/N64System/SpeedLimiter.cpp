#include "stdafx.h"
#include "Project64-core/N64System/SpeedLimiter.h"

#include <Common/Util.h>

#ifdef RETROACHIEVEMENTS
#include "../RAInterface/RA_Interface.h"
#endif

#ifdef _WIN32
#pragma comment(lib, "winmm.lib")

#include <timeapi.h>

typedef LONG(NTAPI* pSetTimerResolution)(ULONG RequestedResolution, BOOLEAN Set, PULONG ActualResolution);
typedef LONG(NTAPI* pQueryTimerResolution)(PULONG MinimumResolution, PULONG MaximumResolution, PULONG CurrentResolution);

static void setAccurateTimerResolution()
{
    timeBeginPeriod(1);

    LONG status;
    pSetTimerResolution setFunction;
    pQueryTimerResolution queryFunction;
    ULONG minResolution, maxResolution, actualResolution;
    const HINSTANCE hLibrary = LoadLibrary(L"NTDLL.dll");
    if (hLibrary == NULL)
    {
        return;
    }

    queryFunction = (pQueryTimerResolution)GetProcAddress(hLibrary, "NtQueryTimerResolution");
    if (queryFunction == NULL)
    {
        return;
    }

    queryFunction(&minResolution, &maxResolution, &actualResolution);
    {
        wchar_t line[256];
        wsprintf(line, L"Win32 Timer Resolution:\n\tMinimum Value:\t%u\n\tMaximum Value:\t%u\n\tActual Value:\t%u\n\n", minResolution, maxResolution, actualResolution);
        OutputDebugString(line);
    }

    setFunction = (pSetTimerResolution)GetProcAddress(hLibrary, "NtSetTimerResolution");
    if (setFunction == NULL)
    {
        return;
    }

    status = setFunction(maxResolution, TRUE, &actualResolution);
    OutputDebugString(status == 0 ? L"Timer Resolution Set\n" : L"Timer Resolution Not Set\n");
}

static void unsetAccurateTimerResolution()
{
    timeEndPeriod(1);
}
#endif

const uint32_t CSpeedLimiter::m_DefaultSpeed = 60;

CSpeedLimiter::CSpeedLimiter() :
    m_Speed(m_DefaultSpeed),
    m_ReanchorPending(true),
    m_NtscRate(false),
    m_BaseSpeed(m_DefaultSpeed),
    m_Frequency(1),
    m_ActiveSpeed(m_DefaultSpeed),
    m_ActiveNtsc(false),
    m_Anchor(0),
    m_FramesSinceAnchor(0),
    m_FpsWindowStart(0),
    m_FpsFrames(0)
{
#ifdef _WIN32
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    m_Frequency = Frequency.QuadPart;

    setAccurateTimerResolution();
    m_Timer = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
    if (!m_Timer)
        m_Timer = CreateWaitableTimer(nullptr, FALSE, nullptr);
#else
    m_Frequency = 1000000;
#endif
}

CSpeedLimiter::~CSpeedLimiter()
{
#ifdef _WIN32
    if (m_Timer)
    {
        CloseHandle(m_Timer);
    }
    unsetAccurateTimerResolution();
#endif
}

int64_t CSpeedLimiter::Ticks(void)
{
#ifdef _WIN32
    LARGE_INTEGER Value;
    QueryPerformanceCounter(&Value);
    return Value.QuadPart;
#else
    HighResTimeStamp Time;
    Time.SetToNow();
    return (int64_t)Time.GetMicroSeconds();
#endif
}

void CSpeedLimiter::SetHertz(uint32_t Hertz)
{
    m_Speed.store(Hertz, std::memory_order_relaxed);
    m_BaseSpeed = Hertz;
    FixSpeedRatio();
}

void CSpeedLimiter::FixSpeedRatio()
{
    m_ReanchorPending.store(true, std::memory_order_relaxed);
}

void CSpeedLimiter::Anchor(int64_t Now)
{
    m_Anchor = Now;
    m_FramesSinceAnchor = 0;
}

// Sleeps on the high resolution timer until the deadline. A late wake isn't
// lost: the next deadline stays where it was, so the next wait is shorter.
void CSpeedLimiter::WaitUntil(int64_t Deadline)
{
    int64_t Remaining = Deadline - Ticks();
    if (Remaining <= 0)
    {
        return;
    }
#ifdef _WIN32
    LARGE_INTEGER DueTime;
    DueTime.QuadPart = -(LONGLONG)(Remaining * 10000000 / m_Frequency);
    if (DueTime.QuadPart < 0 && m_Timer != nullptr && SetWaitableTimer(m_Timer, &DueTime, 0, nullptr, nullptr, FALSE))
    {
        WaitForSingleObject(m_Timer, INFINITE);
    }
    else
    {
        Sleep((DWORD)((Remaining * 1000 + m_Frequency - 1) / m_Frequency));
    }
#else
    pjutil::Sleep((uint32_t)(Remaining / 1000) + 1);
#endif
}

bool CSpeedLimiter::CountFrame(int64_t Now, uint32_t * const FrameRate, uint32_t * const FrameRateFraction)
{
    if (m_FpsWindowStart == 0)
    {
        m_FpsWindowStart = Now;
        m_FpsFrames = 0;
        return false;
    }

    m_FpsFrames += 1;
    int64_t Elapsed = Now - m_FpsWindowStart;
    if (Elapsed < m_Frequency)
    {
        return false;
    }

    uint64_t CentiFps = (uint64_t)m_FpsFrames * (uint64_t)m_Frequency * 100 / (uint64_t)Elapsed;
    if (FrameRate != nullptr) { *FrameRate = (uint32_t)(CentiFps / 100); }
    if (FrameRateFraction != nullptr) { *FrameRateFraction = (uint32_t)(CentiFps % 100); }
    m_FpsWindowStart = Now;
    m_FpsFrames = 0;
    return true;
}

bool CSpeedLimiter::Timer_Process(uint32_t * FrameRate, uint32_t * FrameRateFraction)
{
    int64_t Now = Ticks();
    uint32_t Speed = m_Speed.load(std::memory_order_relaxed);
    if (Speed == 0)
    {
        Speed = 1;
    }

    // Start a fresh schedule on this frame without waiting: after a reset, a
    // savestate load or a speed change, sleeping a whole period on top of the
    // work this frame has already done would show up as a long frame.
    bool Ntsc = m_NtscRate.load(std::memory_order_relaxed) && m_BaseSpeed == m_DefaultSpeed;
    if (m_ReanchorPending.exchange(false, std::memory_order_relaxed) || Speed != m_ActiveSpeed || Ntsc != m_ActiveNtsc || m_Anchor == 0)
    {
        m_ActiveSpeed = Speed;
        m_ActiveNtsc = Ntsc;
        Anchor(Now);
        return CountFrame(Now, FrameRate, FrameRateFraction);
    }

    // Deadlines are an exact fraction of a second from the anchor, so the
    // schedule never drifts and an oversleep on one frame is paid back by a
    // shorter wait on the next.
    // At the NTSC rate every period is stretched by 1001/1000.
    m_FramesSinceAnchor += 1;
    uint64_t Numerator = Ntsc ? 1001 : 1000;
    int64_t Period = (int64_t)((uint64_t)m_Frequency * Numerator / ((uint64_t)Speed * 1000));
    int64_t Deadline = m_Anchor + (int64_t)(m_FramesSinceAnchor * (uint64_t)m_Frequency * Numerator / ((uint64_t)Speed * 1000));
    if (Now < Deadline)
    {
        WaitUntil(Deadline);
        Now = Ticks();
    }
    else
    {
        // A frame that overran by less than a period is absorbed by not
        // waiting. Anything bigger was a stall; restart the schedule from
        // here instead of bursting frames out back to back to catch up.
        if (Now - Deadline > Period)
        {
            Anchor(Now);
        }
    }
    return CountFrame(Now, FrameRate, FrameRateFraction);
}

void CSpeedLimiter::AlterSpeed(const ESpeedChange SpeedChange)
{
    int32_t SpeedFactor = 1;
    if (SpeedChange == DECREASE_SPEED) { SpeedFactor = -1; }

    uint32_t Speed = m_Speed.load(std::memory_order_relaxed);
    if (Speed >= m_DefaultSpeed)
    {
        Speed += 10 * SpeedFactor;
    }
    else if (Speed >= 15)
    {
        Speed += 5 * SpeedFactor;
    }
    else if ((Speed > 1 && SpeedChange == DECREASE_SPEED) || SpeedChange == INCREASE_SPEED)
    {
        Speed += 1 * SpeedFactor;
    }

#ifdef RETROACHIEVEMENTS
    if (Speed < m_DefaultSpeed && RA_HardcoreModeIsActive())
        Speed = m_DefaultSpeed;
#endif

    m_Speed.store(Speed, std::memory_order_relaxed);
    SpeedChanged(Speed);
    FixSpeedRatio();
}

void CSpeedLimiter::SetSpeed(int Speed)
{
    if (Speed < 1)
    {
        Speed = 1;
    }

#ifdef RETROACHIEVEMENTS
    if (Speed < m_DefaultSpeed && RA_HardcoreModeIsActive())
        Speed = m_DefaultSpeed;
#endif

    m_Speed.store(Speed, std::memory_order_relaxed);
    SpeedChanged(Speed);
    FixSpeedRatio();
}

void CSpeedLimiter::SetNtscRate(bool Ntsc)
{
    m_NtscRate.store(Ntsc, std::memory_order_relaxed);
}

int CSpeedLimiter::GetSpeed(void) const
{
    return m_Speed.load(std::memory_order_relaxed);
}

int CSpeedLimiter::GetBaseSpeed(void) const
{
    return m_BaseSpeed;
}

void CSpeedLimiter::AdjustTime(HighResTimeStamp diff)
{
    m_Anchor += (int64_t)diff.Raw();
}

void CSpeedLimiter::Reset()
{
    m_ReanchorPending.store(true, std::memory_order_relaxed);
    m_FpsWindowStart = 0;
}
