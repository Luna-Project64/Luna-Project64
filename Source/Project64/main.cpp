#include "stdafx.h"
#include <Common/HighResTimeStamp.h>
#include <Common/WinEscape.h>
#include <Project64-core/AppInit.h>
#include <Project64-core/VersionLuna.h>
#include "UserInterface/WelcomeScreen.h"
#include "Settings/UISettings.h"

#define MAX_PATH_LENGTH 1024

extern bool DarkModeEnter(DWORD reason);

#ifdef RETROACHIEVEMENTS
#include <Project64-core/RetroAchievements.h>
#endif

extern void setupExceptionFilters();

extern "C" {
    __declspec(dllexport) DWORD NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

extern bool ShadowRamEnabled;

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgs*/, int /*nWinMode*/)
{
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    HighResTimeStamp::Init();
    WinEscape::Init();

    {
        static char currentPath[MAX_PATH_LENGTH];
        if (GetModuleFileNameA(NULL, currentPath, MAX_PATH_LENGTH) == 0) {
            MessageBox(NULL, L"Failed to get current executable path.", L"Updater error", MB_OK);
        }
        char* lastBackslash = strrchr(currentPath, '\\');
        if (lastBackslash != NULL) {
            *lastBackslash = '\0';
        }
        SetCurrentDirectoryA(currentPath);
    }

    try
    {
        AppInit(&Notify(), CPath(CPath::MODULE_DIRECTORY), __argc, __argv);
        setupExceptionFilters();

        if (g_Settings->LoadBool((SettingID)Setting_DarkTheme)) {
            try {
                DarkModeEnter(DLL_PROCESS_ATTACH);
            }
            catch (...) { }
        }

        // Create the main window with menu

        WriteTrace(TraceUserInterface, TraceDebug, "Create main window");
        ProfileManager profileManager("profiles.ini");
        CMainGui MainWindow(true, "Luna's Project64 v" VERSION_LUNA VERSION_LUNA_HOTFIX, profileManager), HiddenWindow(false, "", profileManager);

#ifdef RETROACHIEVEMENTS
        bool raInitAsync = true;
        if (g_Settings->LoadBool((SettingID)Setting_RetroAchievements))
        {
            RA_Init(reinterpret_cast<HWND>(MainWindow.GetWindowHandle()));
            ShadowRamEnabled = true;
        }
#endif

        CMainMenu MainMenu(&MainWindow, profileManager);
        CDebuggerUI Debugger;
        g_Debugger = &Debugger;
        g_Plugins->SetRenderWindows(&MainWindow, &HiddenWindow);
        Notify().SetMainWindow(&MainWindow);

        if (!g_Lang->IsLanguageLoaded())
        {
            WelcomeScreen().DoModal();
        }

        bool isROMLoaded = false;

        if (g_Settings->LoadStringVal(Cmd_RomFile).length() > 0 && g_Settings->LoadStringVal(Cmd_ComboDiskFile).length() > 0)
        {
            // Handle combo loading (N64 ROM and 64DD Disk)

            MainWindow.Show(true);	// Show the main window

            stdstr extcombo = CPath(g_Settings->LoadStringVal(Cmd_ComboDiskFile)).GetExtension();
            stdstr ext = CPath(g_Settings->LoadStringVal(Cmd_RomFile)).GetExtension();

            if (g_Settings->LoadStringVal(Cmd_ComboDiskFile).length() > 0
                && ((_stricmp(extcombo.c_str(), "ndd") == 0) || (_stricmp(extcombo.c_str(), "d64") == 0)))
            {
                if ((!(_stricmp(ext.c_str(), "ndd") == 0)) && (!(_stricmp(ext.c_str(), "d64") == 0)))
                {
                    // Cmd_ComboDiskFile must be a 64DD disk image
                    // Cmd_RomFile must be an N64 ROM image
                    RA_AttemptLogin(true); raInitAsync = false;
                    isROMLoaded = CN64System::RunDiskComboImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str(), g_Settings->LoadStringVal(Cmd_ComboDiskFile).c_str());
                }
            }
        }
        else if (g_Settings->LoadStringVal(Cmd_RomFile).length() > 0)
        {
            // Handle single game (N64 ROM or 64DD Disk)

            MainWindow.Show(true);	// Show the main window

            stdstr ext = CPath(g_Settings->LoadStringVal(Cmd_RomFile)).GetExtension();
            RA_AttemptLogin(true); raInitAsync = false;
            if ((!(_stricmp(ext.c_str(), "ndd") == 0)) && (!(_stricmp(ext.c_str(), "d64") == 0)))
            {
                // File extension is not *.ndd/*.d64 so it should be an N64 ROM
                isROMLoaded = CN64System::RunFileImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str());
            }
            else
            {
                // File extension is *.ndd/*.d64, so it should be an N64 disk image
                isROMLoaded = CN64System::RunDiskImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str());
            }
        }

        if (!isROMLoaded)
        {
            if (UISettingsLoadBool(RomBrowser_Enabled))
            {
                WriteTrace(TraceUserInterface, TraceDebug, "Show ROM browser");
                MainWindow.ShowRomList();
                MainWindow.Show(true);
                MainWindow.HighLightLastRom();
            }
            else
            {
                WriteTrace(TraceUserInterface, TraceDebug, "Show main window");
                MainWindow.Show(true);
            }
        }

        if (raInitAsync)
            RA_AttemptLogin(false);

        WriteTrace(TraceUserInterface, TraceDebug, "Entering message loop");
        MainWindow.ProcessAllMessages();
        WriteTrace(TraceUserInterface, TraceDebug, "Message loop finished");

#ifdef RETROACHIEVEMENTS
        RA_Shutdown();
#endif

        if (g_BaseSystem)
        {
            g_BaseSystem->CloseCpu();
            delete g_BaseSystem;
            g_BaseSystem = nullptr;
        }
        WriteTrace(TraceUserInterface, TraceDebug, "System closed");
    }
    catch (...)
    {
        WriteTrace(TraceUserInterface, TraceError, "Exception caught (File: \"%s\" Line: %d)", __FILE__, __LINE__);
        MessageBox(nullptr, stdstr_f("Exception caught\nFile: %s\nLine: %d", __FILE__, __LINE__).ToUTF16().c_str(), L"Exception", MB_OK);
    }
    AppCleanup();
    DarkModeEnter(DLL_PROCESS_DETACH);
    CoUninitialize();
    return true;
}