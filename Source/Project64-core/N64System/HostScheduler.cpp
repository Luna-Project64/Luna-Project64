#include "stdafx.h"
#include "HostScheduler.h"

#ifdef _WIN32
#include <Windows.h>
#include <mutex>
#include <vector>

#ifndef PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION
#define PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION 0x4
#endif

namespace
{

// Resolved at runtime so the emulator still starts on systems (or Wine
// versions) that predate these calls.
typedef BOOL(WINAPI * GetSystemCpuSetInformationFn)(PSYSTEM_CPU_SET_INFORMATION, ULONG, PULONG, HANDLE, ULONG);
typedef BOOL(WINAPI * SetProcessDefaultCpuSetsFn)(HANDLE, const ULONG *, ULONG);
typedef BOOL(WINAPI * SetThreadSelectedCpuSetsFn)(HANDLE, const ULONG *, ULONG);
typedef BOOL(WINAPI * SetProcessInformationFn)(HANDLE, PROCESS_INFORMATION_CLASS, LPVOID, DWORD);
typedef BOOL(WINAPI * SetThreadInformationFn)(HANDLE, THREAD_INFORMATION_CLASS, LPVOID, DWORD);

template <typename T>
T Kernel32Function(const char * Name)
{
    HMODULE Module = GetModuleHandleW(L"kernel32.dll");
    return Module != nullptr ? (T)GetProcAddress(Module, Name) : nullptr;
}

// CPU set flag bits, see SYSTEM_CPU_SET_INFORMATION::CpuSet.AllFlags
const BYTE CpuSetAllocated = 0x02;
const BYTE CpuSetAllocatedToTargetProcess = 0x04;
const BYTE CpuSetRealTime = 0x08;

std::mutex g_Lock;
int g_Users = 0;
DWORD g_OriginalPriorityClass = 0;
bool g_CpuSetsApplied = false;
std::vector<ULONG> g_FastCpuSets;
std::string g_Description = "not applied";
thread_local bool t_Boosted = false;

// The ids of the CPU sets in the highest efficiency class, or nothing when
// every core is the same type and there is no slow core to steer away from.
std::vector<ULONG> FastestCpuSets(uint32_t & TotalCpuSets)
{
    std::vector<ULONG> Fastest;
    TotalCpuSets = 0;

    GetSystemCpuSetInformationFn GetInformation = Kernel32Function<GetSystemCpuSetInformationFn>("GetSystemCpuSetInformation");
    if (GetInformation == nullptr)
    {
        return Fastest;
    }

    ULONG Length = 0;
    GetInformation(nullptr, 0, &Length, GetCurrentProcess(), 0);
    if (Length == 0)
    {
        return Fastest;
    }
    std::vector<uint8_t> Buffer(Length);
    if (!GetInformation((PSYSTEM_CPU_SET_INFORMATION)Buffer.data(), Length, &Length, GetCurrentProcess(), 0))
    {
        return Fastest;
    }

    BYTE FastestClass = 0;
    for (ULONG Offset = 0; Offset + sizeof(DWORD) * 2 <= Length;)
    {
        PSYSTEM_CPU_SET_INFORMATION Info = (PSYSTEM_CPU_SET_INFORMATION)(Buffer.data() + Offset);
        if (Info->Size == 0)
        {
            break;
        }
        if (Info->Type == CpuSetInformation)
        {
            TotalCpuSets += 1;
            if (Info->CpuSet.EfficiencyClass > FastestClass)
            {
                FastestClass = Info->CpuSet.EfficiencyClass;
            }
        }
        Offset += Info->Size;
    }

    for (ULONG Offset = 0; Offset + sizeof(DWORD) * 2 <= Length;)
    {
        PSYSTEM_CPU_SET_INFORMATION Info = (PSYSTEM_CPU_SET_INFORMATION)(Buffer.data() + Offset);
        if (Info->Size == 0)
        {
            break;
        }
        if (Info->Type == CpuSetInformation && Info->CpuSet.EfficiencyClass == FastestClass)
        {
            BYTE Flags = Info->CpuSet.AllFlags;
            bool ReservedElsewhere = (Flags & CpuSetAllocated) != 0 && (Flags & CpuSetAllocatedToTargetProcess) == 0;
            if (!ReservedElsewhere && (Flags & CpuSetRealTime) == 0)
            {
                Fastest.push_back(Info->CpuSet.Id);
            }
        }
        Offset += Info->Size;
    }

    if (Fastest.size() < 2 || Fastest.size() >= TotalCpuSets)
    {
        Fastest.clear();
    }
    return Fastest;
}

bool SetProcessPowerThrottling(bool Throttle)
{
    SetProcessInformationFn SetInformation = Kernel32Function<SetProcessInformationFn>("SetProcessInformation");
    if (SetInformation == nullptr)
    {
        return false;
    }

    PROCESS_POWER_THROTTLING_STATE State = {};
    State.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
    if (Throttle)
    {
        // Hand control back to the system
        State.ControlMask = 0;
        State.StateMask = 0;
        return SetInformation(GetCurrentProcess(), ProcessPowerThrottling, &State, sizeof(State)) != 0;
    }

    // Never run at efficiency QoS, and keep honouring the timer resolution
    // even while the window is minimised or covered.
    State.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED | PROCESS_POWER_THROTTLING_IGNORE_TIMER_RESOLUTION;
    State.StateMask = 0;
    if (SetInformation(GetCurrentProcess(), ProcessPowerThrottling, &State, sizeof(State)))
    {
        return true;
    }

    // Older Windows 10 does not know the timer resolution flag
    State.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
    return SetInformation(GetCurrentProcess(), ProcessPowerThrottling, &State, sizeof(State)) != 0;
}

void ApplyProcessSettings(void)
{
    HANDLE Process = GetCurrentProcess();
    std::string Description;

    g_OriginalPriorityClass = GetPriorityClass(Process);
    if (g_OriginalPriorityClass == HIGH_PRIORITY_CLASS || g_OriginalPriorityClass == REALTIME_PRIORITY_CLASS)
    {
        Description += "priority=unchanged";
    }
    else
    {
        Description += SetPriorityClass(Process, HIGH_PRIORITY_CLASS) ? "priority=high" : "priority=failed";
    }

    Description += SetProcessPowerThrottling(false) ? " power-throttling=off" : " power-throttling=unchanged";

    uint32_t TotalCpuSets = 0;
    g_FastCpuSets = FastestCpuSets(TotalCpuSets);
    SetProcessDefaultCpuSetsFn SetDefaultCpuSets = Kernel32Function<SetProcessDefaultCpuSetsFn>("SetProcessDefaultCpuSets");
    if (!g_FastCpuSets.empty() && SetDefaultCpuSets != nullptr &&
        SetDefaultCpuSets(Process, g_FastCpuSets.data(), (ULONG)g_FastCpuSets.size()))
    {
        g_CpuSetsApplied = true;
        Description += stdstr_f(" cores=fastest %d of %d", (int)g_FastCpuSets.size(), (int)TotalCpuSets);
    }
    else
    {
        g_FastCpuSets.clear();
        Description += stdstr_f(" cores=all %d", (int)TotalCpuSets);
    }
    g_Description = Description;
}

void RestoreProcessSettings(void)
{
    HANDLE Process = GetCurrentProcess();
    if (g_OriginalPriorityClass != 0)
    {
        SetPriorityClass(Process, g_OriginalPriorityClass);
        g_OriginalPriorityClass = 0;
    }
    SetProcessPowerThrottling(true);
    if (g_CpuSetsApplied)
    {
        SetProcessDefaultCpuSetsFn SetDefaultCpuSets = Kernel32Function<SetProcessDefaultCpuSetsFn>("SetProcessDefaultCpuSets");
        if (SetDefaultCpuSets != nullptr)
        {
            SetDefaultCpuSets(Process, nullptr, 0);
        }
        g_CpuSetsApplied = false;
    }
    g_FastCpuSets.clear();
}

void BoostCurrentThread(void)
{
    HANDLE Thread = GetCurrentThread();
    SetThreadPriority(Thread, THREAD_PRIORITY_HIGHEST);

    SetThreadInformationFn SetInformation = Kernel32Function<SetThreadInformationFn>("SetThreadInformation");
    if (SetInformation != nullptr)
    {
        THREAD_POWER_THROTTLING_STATE State = {};
        State.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
        State.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
        State.StateMask = 0;
        SetInformation(Thread, ThreadPowerThrottling, &State, sizeof(State));
    }

    // The process default only reaches threads created from here on, so
    // the emulation thread gets the fast cores explicitly.
    if (!g_FastCpuSets.empty())
    {
        SetThreadSelectedCpuSetsFn SetThreadCpuSets = Kernel32Function<SetThreadSelectedCpuSetsFn>("SetThreadSelectedCpuSets");
        if (SetThreadCpuSets != nullptr)
        {
            SetThreadCpuSets(Thread, g_FastCpuSets.data(), (ULONG)g_FastCpuSets.size());
        }
    }
}

} // namespace

void CHostScheduler::EnterEmulation(void)
{
    if (g_Settings->LoadBool(Setting_HighPerformanceScheduling))
    {
        std::lock_guard<std::mutex> Guard(g_Lock);
        if (g_Users++ == 0)
        {
            ApplyProcessSettings();
        }
        BoostCurrentThread();
        t_Boosted = true;
    }

    if (g_Settings->LoadBool(Setting_CN64TimeCritical))
    {
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    }
}

void CHostScheduler::LeaveEmulation(void)
{
    if (!t_Boosted)
    {
        return;
    }
    t_Boosted = false;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);

    std::lock_guard<std::mutex> Guard(g_Lock);
    if (--g_Users == 0)
    {
        RestoreProcessSettings();
    }
}

std::string CHostScheduler::Describe(void)
{
    std::lock_guard<std::mutex> Guard(g_Lock);
    return g_Users > 0 ? g_Description : std::string("off");
}

#else

void CHostScheduler::EnterEmulation(void)
{
}

void CHostScheduler::LeaveEmulation(void)
{
}

std::string CHostScheduler::Describe(void)
{
    return "unsupported";
}

#endif
