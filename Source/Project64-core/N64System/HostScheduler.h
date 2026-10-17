#pragma once

#include <string>

// Keeps the host from taking CPU time away from the emulation thread while a
// game is running: raises the process and thread priority, opts out of Windows
// power throttling (EcoQoS, and the timer resolution being ignored while the
// window is occluded) and, on hybrid CPUs, keeps the process on the fastest
// cores so the scheduler can't park the emulation thread on an efficiency core.
//
// Controlled by "High Performance Scheduling" in the [Settings] section of the
// config. Everything here degrades to a no-op where the OS lacks the API.
class CHostScheduler
{
public:
    // Called on the emulation thread before the first frame and after the last.
    static void EnterEmulation(void);
    static void LeaveEmulation(void);

    // One line describing what was applied, for the frame pacing log.
    static std::string Describe(void);
};
