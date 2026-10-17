#pragma once
#include "resource.h"
#include "PlayTime.h"

class CPlayStatsDlg :
    public CDialogImpl<CPlayStatsDlg>
{
public:
    BEGIN_MSG_MAP_EX(CPlayStatsDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        NOTIFY_HANDLER_EX(IDC_STATS_LIST, LVN_COLUMNCLICK, OnColumnClick)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    enum { IDD = IDD_Play_Stats };

    CPlayStatsDlg(void);

private:
    enum
    {
        Column_Name,
        Column_PlayTime,
        Column_Sessions,
        Column_Longest,
        Column_LastPlayed,
    };

    CPlayStatsDlg(const CPlayStatsDlg &);
    CPlayStatsDlg & operator=(const CPlayStatsDlg &);

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnColumnClick(LPNMHDR pnmh);
    LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    void SortList(void);
    static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    std::vector<CPlayTime::GameStats> m_Games;
    CListViewCtrl m_List;
    int m_SortColumn;
    bool m_SortAscending;
};
