#pragma once
#include "resource.h"

class CSaveStateManagerDlg :
    public CDialogImpl<CSaveStateManagerDlg>
{
public:
    BEGIN_MSG_MAP_EX(CSaveStateManagerDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_TIMER, OnTimer)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        NOTIFY_HANDLER_EX(IDC_SS_LIST, LVN_ITEMCHANGED, OnItemChanged)
        NOTIFY_HANDLER_EX(IDC_SS_LIST, NM_DBLCLK, OnDoubleClick)
        COMMAND_ID_HANDLER(IDC_SS_SET_LABEL, OnSetLabel)
        COMMAND_ID_HANDLER(IDC_SS_LOAD, OnLoad)
        COMMAND_ID_HANDLER(IDC_SS_SAVE, OnSave)
        COMMAND_ID_HANDLER(IDOK, OnSetLabel)
        COMMAND_ID_HANDLER(IDCANCEL, OnClose)
    END_MSG_MAP()

    enum { IDD = IDD_Save_States };

    CSaveStateManagerDlg(void);

private:
    enum
    {
        SlotCount = 100,
        Timer_Refresh = 1,
    };
    enum
    {
        Column_Slot,
        Column_Label,
        Column_Saved,
    };

    CSaveStateManagerDlg(const CSaveStateManagerDlg &);
    CSaveStateManagerDlg & operator=(const CSaveStateManagerDlg &);

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnItemChanged(LPNMHDR pnmh);
    LRESULT OnDoubleClick(LPNMHDR pnmh);
    LRESULT OnSetLabel(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnLoad(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnSave(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnClose(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    void PlaceBesideMainWindow(void);
    void RefreshSlot(int Slot);
    void ShowSlotDetails(int Slot);
    int SelectedSlot(void);
    void LoadSlot(int Slot);
    HBITMAP ScaleToPreview(HBITMAP hSource);

    static std::wstring SlotName(int Slot);
    static bool GetSavedTime(int Slot, std::wstring & Time);

    CListViewCtrl m_List;
    HBITMAP m_Preview;
    int m_RefreshCount;
};
