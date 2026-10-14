#pragma once

#include <Project64-core/Settings/GameSettings.h>

#include <Common/HighResTimeStamp.h>

#include <atomic>

#ifdef _WIN32
#include <Windows.h>
#endif

class CSpeedLimiter :
    private CGameSettings
{
public:
	enum ESpeedChange { INCREASE_SPEED, DECREASE_SPEED };

    // What the last call to Timer_Process did, for the frame pacing log.
    // All values are QueryPerformanceCounter ticks.
    struct WaitInfo
    {
        int64_t Requested;  // how long the frame needed to wait for its deadline
        int64_t Actual;     // how long the wait really took
        int64_t Late;       // how far past its deadline the frame already was (0 if it waited)
        bool Reanchored;    // the schedule restarted at this frame
    };

    CSpeedLimiter();
    ~CSpeedLimiter();

    void SetHertz(const uint32_t Hertz);

    // Holds the emulation thread until this frame's slot in the schedule.
    // Returns true roughly once a second, with the measured rate in FrameRate
    // (whole part) and FrameRateFraction (hundredths).
    bool Timer_Process(uint32_t * const FrameRate, uint32_t * const FrameRateFraction = nullptr);

	void AlterSpeed(const ESpeedChange SpeedChange);

    void SetSpeed(int Speed);

    // Run 60Hz games at the NTSC broadcast rate, 60/1.001 = 59.94 frames
    // per second, instead of an even 60. Other base rates are unaffected.
    void SetNtscRate(bool Ntsc);
    int GetSpeed(void) const;
    int GetBaseSpeed(void) const;
    void AdjustTime(HighResTimeStamp);
    void Reset();

    const WaitInfo & LastWait(void) const { return m_LastWait; }

    static HighResTimeStamp Now(void)
    {
        HighResTimeStamp CurrentTime;
		CurrentTime.SetToNow();
		return CurrentTime;
    }

private:
    CSpeedLimiter(const CSpeedLimiter&);
    CSpeedLimiter& operator=(const CSpeedLimiter&);

    void FixSpeedRatio();
    void Anchor(int64_t Now);
    void WaitUntil(int64_t Deadline);
    bool CountFrame(int64_t Now, uint32_t * const FrameRate, uint32_t * const FrameRateFraction);

    static int64_t Ticks(void);

    // Written by the UI thread (speed hotkeys, savestate code), read by the emulation thread
    std::atomic<uint32_t> m_Speed;
    std::atomic<bool> m_ReanchorPending;
    std::atomic<bool> m_NtscRate;
    uint32_t m_BaseSpeed;

    // Owned by the emulation thread
    int64_t m_Frequency;
    uint32_t m_ActiveSpeed;
    bool m_ActiveNtsc;
    int64_t m_Anchor;
    uint64_t m_FramesSinceAnchor;
    int64_t m_FpsWindowStart;
    uint32_t m_FpsFrames;
    int64_t m_SpinMargin;
    int64_t m_PeakOvershoot;
    WaitInfo m_LastWait;

	static const uint32_t m_DefaultSpeed;

#ifdef _WIN32
    HANDLE m_Timer;
#endif
};
