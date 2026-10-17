#include "stdafx.h"
#include <Project64-core/N64System/N64System.h>
#include <Project64-core/Notification.h>
#include <Project64-core/N64System/SummerCart.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/N64System/HostScheduler.h>
#include <Common/Util.h>

void  CN64System::StartEmulationThead()
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
    
    CThread * thread = new CThread((CThread::CTHREAD_START_ROUTINE)StartEmulationThread);
    thread->Start(thread);
    WriteTrace(TraceN64System, TraceDebug, "Done");
}

void CN64System::StartEmulationThread(CThread * thread)
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
    CHostScheduler::EnterEmulation();
#ifdef _WIN32
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
#endif

    EmulationStarting(thread);

#ifdef _WIN32
    CoUninitialize();
#endif
    CHostScheduler::LeaveEmulation();
    WriteTrace(TraceN64System, TraceDebug, "Done");
}

void CN64System::CloseCpu()
{
    WriteTrace(TraceN64System, TraceDebug, "Start");
    if (m_thread == nullptr)
    {
        return;
    }

    WriteTrace(TraceN64System, TraceDebug, "Setting end emulation");
    m_EndEmulation = true;
    if (g_Settings->LoadBool(GameRunning_CPU_Paused))
    {
        WriteTrace(TraceN64System, TraceDebug, "Resume CPU");
        m_hPauseEvent.Trigger();
    }

    if (CThread::GetCurrentThreadId() == m_thread->ThreadID())
    {
        WriteTrace(TraceN64System, TraceDebug, "CloseCpu called on emulation thread");
        ExternalEvent(SysEvent_CloseCPU);
        return;
    }

    CThread * hThread = m_thread;
    m_thread = nullptr;
    for (int count = 0; count < 200; count++)
    {
        if (hThread == nullptr || !hThread->isRunning())
        {
            WriteTrace(TraceN64System, TraceDebug, "Thread no longer running");
            break;
        }
        WriteTrace(TraceN64System, TraceDebug, "%d - waiting", count);
        pjutil::Sleep(100);
        WriteTrace(TraceN64System, TraceDebug, "%d - Finished wait", count);
        if (g_Notify->ProcessGuiMessages())
        {
            return;
        }
    }
    CpuStopped();
    WriteTrace(TraceN64System, TraceDebug, "Deleting thread object");
    delete hThread;
    WriteTrace(TraceN64System, TraceDebug, "Done");
    g_SummerCart->Reset();
}