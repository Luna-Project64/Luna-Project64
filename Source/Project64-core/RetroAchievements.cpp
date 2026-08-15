#include "stdafx.h"

#include "RetroAchievements.h"

#include "../../RAInterface/RA_Consoles.h"
#include "../../RAInterface/RA_Emulators.h"
#include <Project64-core/N64System/Enhancement/Enhancements.h>
#include <Project64-core/N64System/N64System.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugin.h>
#include <Project64-core/Settings.h>
#include <Project64-core/N64System/Mips/SystemEvents.h>
#include <Project64-core/VersionLuna.h>
#include <Project64/UserInterface/Debugger/debugger.h>

static HWND g_hWnd = nullptr;
static const char* g_sFileBeingLoaded = nullptr;
static unsigned int g_nGameId = 0;

static void __cdecl CauseUnpause()
{
    g_BaseSystem->ExternalEvent(SysEvent_ResumeCPU_FromMenu);
}

static void __cdecl CausePause()
{
    g_BaseSystem->ExternalEvent(SysEvent_PauseCPU_FromMenu);
}

static int __cdecl GetMenuItemIndex(HMENU hMenu, const char* pItemName)
{
    int nIndex = 0;
    char pBuffer[256];

    while (nIndex < GetMenuItemCount(hMenu))
    {
        if (GetMenuStringA(hMenu, nIndex, pBuffer, sizeof(pBuffer)-1, MF_BYPOSITION))
        {
            if (!strcmp(pItemName, pBuffer))
                return nIndex;
        }
        nIndex++;
    }

    return -1;
}

extern void NotifyHardcore(bool hc);
void RA_RebuildMenu()
{
    NotifyHardcore(RA_HardcoreModeIsActive() != 0);

    HMENU hMainMenu = GetMenu(g_hWnd);
    if (!hMainMenu)
        return;

    // if RetroAchievements submenu exists, destroy it
    int index = GetMenuItemIndex(hMainMenu, "&RetroAchievements");
    if (index >= 0)
        DeleteMenu(hMainMenu, index, MF_BYPOSITION);

    // append RetroAchievements menu
    AppendMenu(hMainMenu, MF_POPUP|MF_STRING, (UINT_PTR)RA_CreatePopupMenu(), TEXT("&RetroAchievements"));

    // repaint
    DrawMenuBar(g_hWnd);
}

static void __cdecl GetEstimatedGameTitle(char* sNameOut)
{
    if (g_sFileBeingLoaded != nullptr)
    {
        const char* sLastSlash = nullptr;
        const char* sFilename = g_sFileBeingLoaded;
        while (*sFilename) {
            if (*sFilename == '\\')
                sLastSlash = sFilename;

            sFilename++;
        }

        sFilename = (sLastSlash) ? (sLastSlash + 1) : g_sFileBeingLoaded;
        strncpy(sNameOut, sFilename, 256);
    }
    else
    {
        strncpy(sNameOut, "?", 256);
    }
}

void RA_IdentifyGame(const char* sFilename, uint8_t* pData, size_t nSize)
{
    g_sFileBeingLoaded = sFilename;
    g_nGameId = RA_IdentifyRom(pData, nSize);
    g_sFileBeingLoaded = nullptr;
}

static void __cdecl ResetEmulator()
{
    if (!g_BaseSystem)
        return;

    if (RA_HardcoreModeIsActive())
    {
        // close all of the debugger windows
        ((CDebuggerUI*)g_Debugger)->Debug_Reset();

        // force cheats to be reapplied (hardcore mode will prevent)
        g_Enhancements->UpdateCheats();

        // ensure speed is not lower than default (function will cap at default internally)
        g_BaseSystem->SetSpeed(g_BaseSystem->GetSpeed());
    }

    // reset emulator
    g_BaseSystem->ExternalEvent(SysEvent_ResetCPU_Hard);
}

static void __cdecl LoadROM(const char*) {}

static void __cdecl RebuildMenu()
{
	return RA_RebuildMenu();
}

void RA_Init(HWND hMainWindow)
{
    // initialize the DLL
    RA_InitClient(hMainWindow, "LunaProject64", VERSION_LUNA);
    RA_SetConsoleID(N64);
    g_hWnd = hMainWindow;

    // provide callbacks to the DLL
    RA_InstallSharedFunctions(NULL, CauseUnpause, CausePause, RebuildMenu, GetEstimatedGameTitle, ResetEmulator, LoadROM);

    // add a placeholder menu item and start the login process - menu will be updated when login completes
    RA_RebuildMenu();

    // ensure titlebar text matches expected format
    RA_UpdateAppTitle("");
}

static unsigned int __cdecl RAMBlockReader(unsigned int nAddress, unsigned char* pBuffer, unsigned int nBytes)
{
    memcpy(pBuffer, &g_MMU->Rdram()[nAddress], nBytes);
    return nBytes;
}

static unsigned char __cdecl RAMByteReader(unsigned int nAddress)
{
    return g_MMU->Rdram()[nAddress];
}

static void __cdecl RAMByteWriter(unsigned int nAddress, unsigned char nVal)
{
    g_MMU->Rdram()[nAddress] = nVal;
}

void RA_UpdateMemoryBanks()
{
    RA_ClearMemoryBanks();
    RA_InstallMemoryBank(0, RAMByteReader, RAMByteWriter, g_MMU->RdramSize());
    RA_InstallMemoryBankBlockReader(0, RAMBlockReader);
    RA_ActivateGame(g_nGameId);
}

void RA_ProcessInputs()
{
    BUTTONS Keys;
    memset(&Keys, 0, sizeof(Keys));
    g_Plugins->Control()->GetKeys(0, &Keys); // Get keys from 1st player controller.

    ControllerInput input;
    input.m_bUpPressed = Keys.U_DPAD;
    input.m_bDownPressed = Keys.D_DPAD;
    input.m_bLeftPressed = Keys.L_DPAD;
    input.m_bRightPressed = Keys.R_DPAD;
    input.m_bCancelPressed = Keys.B_BUTTON;
    input.m_bConfirmPressed = Keys.A_BUTTON;
    input.m_bQuitPressed = Keys.START_BUTTON;

    RA_NavigateOverlay(&input);
}
