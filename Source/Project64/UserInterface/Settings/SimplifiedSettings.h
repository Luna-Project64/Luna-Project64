#pragma once
#include "stdafx.h"
#include "SettingsPage.h"

namespace {
    HWND hDlgItem;
    LPARAM lParam;
};

#define KEEP_CURRENT -1

enum InputPlugin {
    PLUGIN_NRAGE,
    PLUGIN_LUNADINPUT,
    PLUGIN_LMAPPER,
    PLUGIN_WIIUGCN,
    PLUGIN_RAPHNETRAW,
};

class SimplifiedSettings :

    public CSettingsPageImpl<SimplifiedSettings>
{
public:
    BEGIN_MSG_MAP_EX(SimplifiedSettings)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
        MSG_WM_ERASEBKGND(OnEraseBackground)
        COMMAND_ID_HANDLER(IDALLGAMES, OnAllGamesCmd)
        COMMAND_ID_HANDLER(IDCURRENTGAME, OnCurrentGameCmd)
        COMMAND_ID_HANDLER(IDHELP, OnHelpCmd)
        COMMAND_ID_HANDLER(IDCURRENTGAME, OnCancelCmd)
        MESSAGE_HANDLER(WM_NOTIFY, OnListNotify)
        END_MSG_MAP()

    enum { IDD = IDD_Welcome };


    void Display(HWND hParent, bool BlockExecution);
    SimplifiedSettings(void);

private:
    SimplifiedSettings(const SimplifiedSettings&);
    SimplifiedSettings& operator=(const SimplifiedSettings&);

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnCtlColorStatic(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    HBRUSH OnCtlColorStatic(CDCHandle dc, CStatic wndStatic);
    BOOL OnEraseBackground(CDCHandle dc);
    LRESULT OnAllGamesCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnCurrentGameCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnHelpCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnCancelCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnListNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    static int CALLBACK SelectDirCallBack(HWND hwnd, DWORD uMsg, DWORD lp, DWORD lpData);

    CBitmapPicture m_Logo;
};

