#include "stdafx.h"
#include <Project64-core/AppInit.h>
#include "UserInterface/WelcomeScreen.h"
#include "Settings/UISettings.h"
#define MAX_PATH_LENGTH 1024

extern bool DarkModeEnter(DWORD reason);

#ifdef RETROACHIEVEMENTS
#include <Project64-core/RetroAchievements.h>
#endif

int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpszArgs*/, int /*nWinMode*/)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    char currentPath[MAX_PATH_LENGTH];
    char updaterExePath[MAX_PATH_LENGTH];
    char updaterExePathZoneIdentifier[MAX_PATH_LENGTH];

    // Zero memory for STARTUPINFO and PROCESS_INFORMATION structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Get the path of the current executable
    if (GetModuleFileNameA(NULL, currentPath, MAX_PATH_LENGTH) == 0) {
        printf("GetModuleFileName failed (%d).\n", GetLastError());
        MessageBox(NULL, L"Failed to get current executable path.", L"Updater error", MB_OK);
    }

    // Remove the executable name from the path to get the directory
    char* lastBackslash = strrchr(currentPath, '\\');
    if (lastBackslash != NULL) {
        *lastBackslash = '\0'; // Terminate the string to get the directory
    }

    // Construct the full path to updater.exe
    snprintf(updaterExePath, MAX_PATH_LENGTH, "%s\\LunaU.exe", currentPath);

    snprintf(updaterExePathZoneIdentifier, MAX_PATH_LENGTH, "%s:Zone.Identifier", updaterExePath);
    DeleteFileA(updaterExePathZoneIdentifier);

    char commandLine[MAX_PATH_LENGTH + 10];  // Adjust size if necessary
    snprintf(commandLine, sizeof(commandLine), "\"%s\" %s", updaterExePath, "v3.5.5");

    // Create the process
    if (!ShellExecuteA(
        NULL,
        "open",
        updaterExePath,
        commandLine,
        0,
        SW_SHOWNORMAL)
    ) {
        // If the function fails, print the error and exit
        int err;
        err = GetLastError();
        printf("CreateProcess failed (%d).\n", GetLastError());
        MessageBox(NULL, L"Failed to create updater process.", L"Updater error", MB_OK);
    }

    // Successfully created the process
    printf("Process launched asynchronously: %s\n", updaterExePath);

    // Close process and thread handles to avoid memory leaks
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    try
    {
        CoInitialize(nullptr);
        AppInit(&Notify(), CPath(CPath::MODULE_DIRECTORY), __argc, __argv);

        if (g_Settings->LoadBool((SettingID)Setting_DarkTheme)) {
            try {
                DarkModeEnter(DLL_PROCESS_ATTACH);
            }
            catch (...) {
                MessageBox(NULL, L"Dark mode isn't supported.", L"Dark theme error", MB_OK);
            }
        }

        // Create the main window with menu
		
        WriteTrace(TraceUserInterface, TraceDebug, "Create main window");
        CMainGui MainWindow(true, stdstr_f("Luna's Project64 v3.5.5").c_str()), HiddenWindow(false);
        CMainMenu MainMenu(&MainWindow);
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
                    isROMLoaded = CN64System::RunDiskComboImage(g_Settings->LoadStringVal(Cmd_RomFile).c_str(), g_Settings->LoadStringVal(Cmd_ComboDiskFile).c_str());
                }
            }
        }
        else if (g_Settings->LoadStringVal(Cmd_RomFile).length() > 0)
        {
            // Handle single game (N64 ROM or 64DD Disk)

            MainWindow.Show(true);	// Show the main window

            stdstr ext = CPath(g_Settings->LoadStringVal(Cmd_RomFile)).GetExtension();
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

#ifdef RETROACHIEVEMENTS
        if (g_Settings->LoadBool((SettingID) Setting_RetroAchievements))
            RA_Init(reinterpret_cast<HWND>(MainWindow.GetWindowHandle()));
#endif

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