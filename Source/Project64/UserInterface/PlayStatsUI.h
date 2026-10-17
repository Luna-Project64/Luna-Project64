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
        NOTIFY_HANDLER_EX(IDC_STATS_LIST, LVN_ITEMCHANGED, OnItemChanged)
        NOTIFY_HANDLER_EX(IDC_STATS_LIST, NM_DBLCLK, OnDoubleClick)
        COMMAND_ID_HANDLER(IDC_STATS_STARS, OnStarTimes)
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
    LRESULT OnItemChanged(LPNMHDR pnmh);
    LRESULT OnDoubleClick(LPNMHDR pnmh);
    LRESULT OnStarTimes(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    void SortList(void);
    int SelectedGame(void);
    void UpdateStarButton(void);
    void ShowStarTimes(void);
    static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    std::vector<CPlayTime::GameStats> m_Games;
    std::vector<std::string> m_Keys;
    CListViewCtrl m_List;
    int m_SortColumn;
    bool m_SortAscending;
};
