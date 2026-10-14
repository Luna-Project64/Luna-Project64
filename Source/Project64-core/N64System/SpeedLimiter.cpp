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

// The frame wait sleeps on a high resolution timer to within this margin of
// the deadline and spins the rest. The timer wakes the thread late by a few
// hundred microseconds on average and by several milliseconds now and then;
// the spin takes that out of every frame. The margin follows the worst
// overshoot seen recently, so a noisy machine spins a little longer instead
// of missing deadlines.
static const int64_t MinSpinMarginUs = 1500;
static const int64_t MaxSpinMarginUs = 4000;
static const int64_t SpinMarginHeadroomUs = 500;
static const int64_t InitialSpinMarginUs = 2000;

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
    m_FpsFrames(0),
    m_SpinMargin(0),
    m_PeakOvershoot(0),
    m_LastWait()
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
    m_PeakOvershoot = (InitialSpinMarginUs - SpinMarginHeadroomUs) * m_Frequency / 1000000;
    m_SpinMargin = InitialSpinMarginUs * m_Frequency / 1000000;
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
    m_Speed = Hertz;
    m_BaseSpeed = Hertz;
    FixSpeedRatio();
}

void CSpeedLimiter::FixSpeedRatio()
{
    m_ReanchorPending = true;
}

void CSpeedLimiter::Anchor(int64_t Now)
{
    m_Anchor = Now;
    m_FramesSinceAnchor = 0;
    m_LastWait.Reanchored = true;
}

void CSpeedLimiter::WaitUntil(int64_t Deadline)
{
#ifdef _WIN32
    int64_t Start = Ticks();
    int64_t Remaining = Deadline - Start;
    if (Remaining > m_SpinMargin)
    {
        int64_t SleepTicks = Remaining - m_SpinMargin;
        LARGE_INTEGER DueTime;
        DueTime.QuadPart = -(LONGLONG)(SleepTicks * 10000000 / m_Frequency);
        if (DueTime.QuadPart < 0 && m_Timer != nullptr && SetWaitableTimer(m_Timer, &DueTime, 0, nullptr, nullptr, FALSE))
        {
            WaitForSingleObject(m_Timer, INFINITE);
        }
        else
        {
            Sleep((DWORD)(SleepTicks * 1000 / m_Frequency));
        }

        // Track how late the timer woke us, decaying slowly so one bad wake
        // widens the margin for a few seconds rather than for good.
        int64_t Overshoot = Ticks() - (Start + SleepTicks);
        m_PeakOvershoot -= m_PeakOvershoot / 256;
        if (Overshoot > m_PeakOvershoot)
        {
            m_PeakOvershoot = Overshoot;
        }
        int64_t Margin = m_PeakOvershoot + SpinMarginHeadroomUs * m_Frequency / 1000000;
        int64_t MinMargin = MinSpinMarginUs * m_Frequency / 1000000;
        int64_t MaxMargin = MaxSpinMarginUs * m_Frequency / 1000000;
        m_SpinMargin = Margin < MinMargin ? MinMargin : (Margin > MaxMargin ? MaxMargin : Margin);
    }
    while (Ticks() < Deadline)
    {
        YieldProcessor();
    }
#else
    int64_t Remaining = Deadline - Ticks();
    if (Remaining > 0)
    {
        pjutil::Sleep((uint32_t)(Remaining / 1000) + 1);
    }
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
    uint32_t Speed = m_Speed;
    if (Speed == 0)
    {
        Speed = 1;
    }

    m_LastWait = WaitInfo();

    // Start a fresh schedule on this frame without waiting: after a reset, a
    // savestate load or a speed change, sleeping a whole period on top of the
    // work this frame has already done would show up as a long frame.
    bool Ntsc = m_NtscRate && m_BaseSpeed == m_DefaultSpeed;
    if (m_ReanchorPending.exchange(false) || Speed != m_ActiveSpeed || Ntsc != m_ActiveNtsc || m_Anchor == 0)
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
        int64_t Woke = Ticks();
        m_LastWait.Requested = Deadline - Now;
        m_LastWait.Actual = Woke - Now;
        Now = Woke;
    }
    else
    {
        // A frame that overran by less than a period is absorbed by not
        // waiting. Anything bigger was a stall; restart the schedule from
        // here instead of bursting frames out back to back to catch up.
        m_LastWait.Late = Now - Deadline;
        if (m_LastWait.Late > Period)
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

    uint32_t Speed = m_Speed;
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

    m_Speed = Speed;
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

    m_Speed = Speed;
    SpeedChanged(Speed);
    FixSpeedRatio();
}

void CSpeedLimiter::SetNtscRate(bool Ntsc)
{
    m_NtscRate = Ntsc;
}

int CSpeedLimiter::GetSpeed(void) const
{
    return m_Speed;
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
    m_ReanchorPending = true;
    m_FpsWindowStart = 0;
}
