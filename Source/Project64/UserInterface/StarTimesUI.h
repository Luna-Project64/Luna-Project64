#pragma once
#include "resource.h"
#include "PlayTime.h"

class CStarTimesDlg :
    public CDialogImpl<CStarTimesDlg>
{
public:
    BEGIN_MSG_MAP_EX(CStarTimesDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        NOTIFY_HANDLER_EX(IDC_STARS_LIST, LVN_COLUMNCLICK, OnColumnClick)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    enum { IDD = IDD_Star_Times };

    CStarTimesDlg(const std::string & GameKey, const std::string & GameName);

    static std::wstring CourseName(int Course);
    static std::wstring StarName(int Key);

private:
    enum
    {
        Column_Course,
        Column_Star,
        Column_Time,
        Column_Grabs,
        Column_FirstGrab,
    };

    struct Row
    {
        int Key;
        CPlayTime::StarStats Stats;
    };

    CStarTimesDlg(const CStarTimesDlg &);
    CStarTimesDlg & operator=(const CStarTimesDlg &);

    LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled);
    LRESULT OnColumnClick(LPNMHDR pnmh);
    LRESULT OnCloseCmd(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL & bHandled);

    static int CALLBACK CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    std::wstring DisplayCourse(int Course);
    std::wstring DisplayStar(int Key);

    bool m_HaveNames;
    CSm64Probe::Names m_Names;

    std::string m_GameKey;
    std::string m_GameName;
    std::vector<Row> m_Rows;
    CListViewCtrl m_List;
    int m_SortColumn;
    bool m_SortAscending;
};
