#pragma once
#include "resource.h"
#include "PlayTime.h"

// A heatmap of play time per day for the last 26 weeks, plus the games played
// on the selected day.
class CPlayCalendarDlg :
    public CDialogImpl<CPlayCalendarDlg>
{
public:
    BEGIN_MSG_MAP_EX(CPlayCalendarDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_DRAWITEM, OnDrawItem)
        COMMAND_HANDLER(IDC_CAL_GRID, STN_CLICKED, OnGridClicked)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    enum { IDD = IDD_Play_Calendar };

    CPlayCalendarDlg(void);

private:
    enum
    {
        Weeks = 26,
    };

    CPlayCalendarDlg(const CPlayCalendarDlg &);
    CPlayCalendarDlg & operator=(const CPlayCalendarDlg &);

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnDrawItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnGridClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);
    LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    // Grid geometry inside the grid control, in pixels
    void Layout(const RECT & rcClient, int & Left, int & Top, int & Cell, int & Gap);
    bool DayAt(POINT pt, int & Index);
    uint32_t DaySeconds(int Index);
    std::string DayKeyAt(int Index);
    void SelectDay(int Index);

    CPlayTime::DailyStatsMap m_Daily;
    CPlayTime::GameStatsMap m_Games;
    time_t m_FirstDay;   // local midnight of the Monday the grid starts on
    int m_TodayIndex;    // index of today, days since m_FirstDay
    int m_Selected;
    CListViewCtrl m_DayList;
};
