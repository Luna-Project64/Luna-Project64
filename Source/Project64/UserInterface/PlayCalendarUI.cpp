#include "stdafx.h"
#include "PlayCalendarUI.h"
#include <algorithm>
#include <time.h>

static struct tm LocalDay(time_t Time)
{
    struct tm Day = { 0 };
    localtime_s(&Day, &Time);
    Day.tm_hour = 12; // midday, so adding days never trips over a DST change
    Day.tm_min = 0;
    Day.tm_sec = 0;
    Day.tm_isdst = -1;
    return Day;
}

static struct tm AddDays(struct tm Day, int Days)
{
    Day.tm_mday += Days;
    Day.tm_isdst = -1;
    mktime(&Day);
    return Day;
}

CPlayCalendarDlg::CPlayCalendarDlg(void) :
    m_FirstDay(0),
    m_TodayIndex(0),
    m_Selected(-1)
{
}

std::string CPlayCalendarDlg::DayKeyAt(int Index)
{
    struct tm First = LocalDay(m_FirstDay);
    struct tm Day = AddDays(First, Index);
    return stdstr_f("%04d-%02d-%02d", Day.tm_year + 1900, Day.tm_mon + 1, Day.tm_mday);
}

uint32_t CPlayCalendarDlg::DaySeconds(int Index)
{
    CPlayTime::DailyStatsMap::const_iterator Day = m_Daily.find(DayKeyAt(Index));
    if (Day == m_Daily.end())
    {
        return 0;
    }
    uint32_t Seconds = 0;
    for (CPlayTime::DayStats::const_iterator Game = Day->second.begin(); Game != Day->second.end(); Game++)
    {
        Seconds += Game->second;
    }
    return Seconds;
}

LRESULT CPlayCalendarDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    m_Daily = CPlayTime::Get().GetDailyStats();
    m_Games = CPlayTime::Get().GetAllStats();

    // Columns are weeks starting on Monday, the last column holds today
    struct tm Today = LocalDay(time(nullptr));
    mktime(&Today);
    int Weekday = (Today.tm_wday + 6) % 7;
    m_TodayIndex = (Weeks - 1) * 7 + Weekday;
    struct tm First = AddDays(Today, -m_TodayIndex);
    m_FirstDay = mktime(&First);

    // Summary: recent totals and streaks
    uint32_t Week = 0, Month = 0;
    int DaysPlayed = 0, Streak = 0, LongestStreak = 0, Run = 0;
    for (int i = 0; i <= m_TodayIndex; i++)
    {
        uint32_t Seconds = DaySeconds(i);
        if (i > m_TodayIndex - 7) { Week += Seconds; }
        if (i > m_TodayIndex - 30) { Month += Seconds; }
        if (Seconds > 0) { DaysPlayed++; Run++; }
        else { Run = 0; }
        LongestStreak = Run > LongestStreak ? Run : LongestStreak;
    }
    // A streak is still alive if today hasn't been played yet
    for (int i = DaySeconds(m_TodayIndex) > 0 ? m_TodayIndex : m_TodayIndex - 1; i >= 0 && DaySeconds(i) > 0; i--)
    {
        Streak++;
    }

    std::wstring Summary;
    if (m_Daily.empty())
    {
        Summary = L"No days logged yet. Days are recorded from now on while you play.";
    }
    else
    {
        Summary = L"Last 7 days: " + CPlayTime::FormatDuration(Week) + L"     Last 30 days: " + CPlayTime::FormatDuration(Month) +
            stdstr_f("\nDays played: %d     Current streak: %d day%s     Longest streak: %d day%s",
                DaysPlayed, Streak, Streak == 1 ? "" : "s", LongestStreak, LongestStreak == 1 ? "" : "s").ToUTF16();
    }
    GetDlgItem(IDC_CAL_SUMMARY).SetWindowText(Summary.c_str());

    m_DayList.Attach(GetDlgItem(IDC_CAL_DAY_LIST));
    m_DayList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    CRect rcList;
    m_DayList.GetClientRect(&rcList);
    int Width = rcList.Width() - GetSystemMetrics(SM_CXVSCROLL);
    m_DayList.InsertColumn(0, L"Game", LVCFMT_LEFT, Width * 75 / 100);
    m_DayList.InsertColumn(1, L"Time", LVCFMT_LEFT, Width * 25 / 100);

    SelectDay(m_TodayIndex);
    return TRUE;
}

void CPlayCalendarDlg::Layout(const RECT & rcClient, int & Left, int & Top, int & Cell, int & Gap)
{
    // Room on the left for weekday names and on top for month names
    HDC hDC = ::GetDC(m_hWnd);
    HGDIOBJ hOld = SelectObject(hDC, GetFont());
    SIZE Text;
    GetTextExtentPoint32W(hDC, L"Wed", 3, &Text);
    SelectObject(hDC, hOld);
    ::ReleaseDC(m_hWnd, hDC);

    Left = Text.cx + 6;
    Top = Text.cy + 4;
    int Width = rcClient.right - rcClient.left - Left;
    int Height = rcClient.bottom - rcClient.top - Top;
    int Pitch = Width / Weeks < Height / 7 ? Width / Weeks : Height / 7;
    Gap = Pitch >= 10 ? 2 : 1;
    Cell = Pitch - Gap;
}

bool CPlayCalendarDlg::DayAt(POINT pt, int & Index)
{
    CRect rcGrid;
    GetDlgItem(IDC_CAL_GRID).GetClientRect(&rcGrid);
    int Left, Top, Cell, Gap;
    Layout(rcGrid, Left, Top, Cell, Gap);
    if (pt.x < Left || pt.y < Top)
    {
        return false;
    }
    int Column = (pt.x - Left) / (Cell + Gap);
    int Row = (pt.y - Top) / (Cell + Gap);
    if (Column >= Weeks || Row >= 7)
    {
        return false;
    }
    Index = Column * 7 + Row;
    return Index <= m_TodayIndex;
}

LRESULT CPlayCalendarDlg::OnDrawItem(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
    if (wParam != IDC_CAL_GRID)
    {
        bHandled = FALSE;
        return FALSE;
    }
    LPDRAWITEMSTRUCT dis = (LPDRAWITEMSTRUCT)lParam;
    CDCHandle dc(dis->hDC);
    CRect rc(dis->rcItem);
    dc.FillSolidRect(&rc, GetSysColor(COLOR_BTNFACE));

    int Left, Top, Cell, Gap;
    Layout(rc, Left, Top, Cell, Gap);
    HFONT hOldFont = dc.SelectFont(GetFont());
    dc.SetBkMode(TRANSPARENT);
    dc.SetTextColor(GetSysColor(COLOR_BTNTEXT));

    // Weekday names on Monday, Wednesday and Friday rows
    const wchar_t * DayNames[] = { L"Mon", nullptr, L"Wed", nullptr, L"Fri", nullptr, nullptr };
    for (int Row = 0; Row < 7; Row++)
    {
        if (DayNames[Row] != nullptr)
        {
            CRect rcText(rc.left, rc.top + Top + Row * (Cell + Gap), rc.left + Left - 4, rc.top + Top + Row * (Cell + Gap) + Cell);
            dc.DrawText(DayNames[Row], -1, &rcText, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    // Purple ramp, from no play to 2 hours or more
    const COLORREF Levels[] = { RGB(235, 237, 240), RGB(217, 200, 245), RGB(180, 141, 235), RGB(138, 85, 214), RGB(91, 43, 160) };
    struct tm First = LocalDay(m_FirstDay);
    int LastMonth = -1;
    for (int Column = 0; Column < Weeks; Column++)
    {
        struct tm WeekStart = AddDays(First, Column * 7);
        if (WeekStart.tm_mon != LastMonth)
        {
            // Month name above the first week that starts in it
            wchar_t Month[16];
            wcsftime(Month, sizeof(Month) / sizeof(Month[0]), L"%b", &WeekStart);
            dc.TextOut(rc.left + Left + Column * (Cell + Gap), rc.top, Month);
            LastMonth = WeekStart.tm_mon;
        }
        for (int Row = 0; Row < 7; Row++)
        {
            int Index = Column * 7 + Row;
            if (Index > m_TodayIndex)
            {
                break;
            }
            uint32_t Minutes = DaySeconds(Index) / 60;
            int Level = Minutes == 0 ? 0 : Minutes < 15 ? 1 : Minutes < 45 ? 2 : Minutes < 120 ? 3 : 4;
            if (Level == 0 && DaySeconds(Index) > 0)
            {
                Level = 1;
            }
            CRect rcCell(rc.left + Left + Column * (Cell + Gap), rc.top + Top + Row * (Cell + Gap), 0, 0);
            rcCell.right = rcCell.left + Cell;
            rcCell.bottom = rcCell.top + Cell;
            dc.FillSolidRect(&rcCell, Levels[Level]);
            if (Index == m_Selected)
            {
                CBrush Frame;
                Frame.CreateSolidBrush(GetSysColor(COLOR_BTNTEXT));
                dc.FrameRect(&rcCell, Frame);
            }
        }
    }
    dc.SelectFont(hOldFont);
    return TRUE;
}

void CPlayCalendarDlg::SelectDay(int Index)
{
    m_Selected = Index;
    GetDlgItem(IDC_CAL_GRID).Invalidate();

    struct tm Day = AddDays(LocalDay(m_FirstDay), Index);
    wchar_t Date[64];
    wcsftime(Date, sizeof(Date) / sizeof(Date[0]), L"%A, %B %#d, %Y", &Day);
    uint32_t Seconds = DaySeconds(Index);
    std::wstring Title = std::wstring(Date) + (Seconds > 0 ? L": " + CPlayTime::FormatDuration(Seconds) : std::wstring(L": nothing played"));
    GetDlgItem(IDC_CAL_DAY_TITLE).SetWindowText(Title.c_str());

    m_DayList.DeleteAllItems();
    CPlayTime::DailyStatsMap::const_iterator Games = m_Daily.find(DayKeyAt(Index));
    if (Games == m_Daily.end())
    {
        return;
    }
    // Most played first
    std::vector<std::pair<uint32_t, std::string> > Sorted;
    for (CPlayTime::DayStats::const_iterator Game = Games->second.begin(); Game != Games->second.end(); Game++)
    {
        CPlayTime::GameStatsMap::const_iterator Info = m_Games.find(Game->first);
        Sorted.push_back(std::make_pair(Game->second, Info != m_Games.end() && !Info->second.Name.empty() ? Info->second.Name : Game->first));
    }
    std::sort(Sorted.rbegin(), Sorted.rend());
    for (size_t i = 0; i < Sorted.size(); i++)
    {
        int Item = m_DayList.InsertItem((int)i, stdstr(Sorted[i].second).ToUTF16().c_str());
        m_DayList.SetItemText(Item, 1, CPlayTime::FormatDuration(Sorted[i].first).c_str());
    }
}

LRESULT CPlayCalendarDlg::OnGridClicked(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    DWORD Pos = GetMessagePos();
    POINT pt = { (short)LOWORD(Pos), (short)HIWORD(Pos) };
    GetDlgItem(IDC_CAL_GRID).ScreenToClient(&pt);
    int Index;
    if (DayAt(pt, Index))
    {
        SelectDay(Index);
    }
    return 0;
}

LRESULT CPlayCalendarDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    EndDialog(IDOK);
    return 0;
}
