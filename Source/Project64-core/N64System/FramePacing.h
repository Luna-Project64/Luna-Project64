#pragma once

#include <stdint.h>

// Frame pacing log: a running account of where each frame's host time went,
// so that a hitch can be pinned on a component after the fact rather than
// guessed at.
//
// A frame here runs from one wake-up of the speed limiter to the next, so on a
// healthy run every frame is exactly one period long no matter how the work
// inside it varies. Any frame that runs past its deadline, or whose wait
// overshoots, is written out as a HITCH line with its cost breakdown, and a
// SUMMARY line follows every 600 frames.
//
// The emulation thread only does arithmetic on fixed storage; formatting and
// file writes happen on a low priority background thread. Output goes to
// FramePacing.log in the log directory. Controlled by "Frame Pacing Log" in
// the [Settings] section of the config.

enum FRAME_COST
{
    FrameCost_Emulation = 0, // everything not listed below: CPU, memory, DMA, timers
    FrameCost_Rsp,           // RSP plugin, including the display and audio lists it runs
    FrameCost_Gfx,           // graphics plugin calls made by the core (UpdateScreen, VI changes, RDP lists)
    FrameCost_Audio,         // audio plugin buffer submissions
    FrameCost_Pif,           // controller plugin polling and EEPROM, via PIF RAM
    FrameCost_Compile,       // recompiler translating new code
    FrameCost_State,         // savestate save or load
    FrameCost_Wait,          // the speed limiter waiting for the frame's deadline
    FrameCost_Count
};

class CFramePacing
{
public:
    // Emulation thread, before the first frame and after the last.
    static void Start(void);
    static void Stop(void);

    // The CPU was paused; the frame it lands in is not a hitch.
    static void Paused(void);

    // What the speed limiter did for this frame, in QueryPerformanceCounter ticks.
    static void RecordLimiter(int64_t Requested, int64_t Actual, int64_t Late, bool Reanchored);

    // End of a frame, just after the speed limiter returns.
    static void FrameEnd(uint32_t TargetHertz);

    // Cost attribution; use CFrameCost rather than calling these directly.
    static bool Enter(FRAME_COST Cost);
    static void Leave(void);
};

// Charges the time spent in its scope to one cost. Nested scopes are
// exclusive: the inner one's time is not also charged to the outer one.
class CFrameCost
{
public:
    explicit CFrameCost(FRAME_COST Cost) :
        m_Entered(CFramePacing::Enter(Cost))
    {
    }

    ~CFrameCost()
    {
        if (m_Entered)
        {
            CFramePacing::Leave();
        }
    }

private:
    CFrameCost(const CFrameCost &);
    CFrameCost & operator=(const CFrameCost &);

    bool m_Entered;
};
