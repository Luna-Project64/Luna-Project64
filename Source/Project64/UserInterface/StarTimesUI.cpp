#include "stdafx.h"
#include "StarTimesUI.h"

CStarTimesDlg::CStarTimesDlg(const std::string & GameKey, const std::string & GameName) :
    m_GameKey(GameKey),
    m_GameName(GameName),
    m_SortColumn(Column_Course),
    m_SortAscending(true)
{
}

std::wstring CStarTimesDlg::CourseName(int Course)
{
    // Hacks reuse the vanilla course slots with their own levels, so stick to slot numbers
    if (Course == 0) { return L"Castle"; }
    if (Course <= 15) { return stdstr_f("Course %d", Course).ToUTF16(); }
    if (Course <= 18) { return stdstr_f("Bowser %d", Course - 15).ToUTF16(); }
    if (Course <= 24) { return stdstr_f("Secret %d", Course - 18).ToUTF16(); }
    return L"Ending";
}

std::wstring CStarTimesDlg::StarName(int Key)
{
    int Star = Key % 8;
    return Star == CPlayTime::StarAnyAct ? L"(no star select)" : stdstr_f("Star %d", Star + 1).ToUTF16();
}

LRESULT CStarTimesDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    SetWindowText(stdstr_f("Star Times - %s", m_GameName.c_str()).ToUTF16().c_str());

    CPlayTime::StarStatsMap Stars = CPlayTime::Get().GetStarStats(m_GameKey);
    uint32_t TotalSeconds = 0, TotalGrabs = 0;
    const Row * MostTime = nullptr;
    for (CPlayTime::StarStatsMap::const_iterator itr = Stars.begin(); itr != Stars.end(); itr++)
    {
        if (itr->second.Seconds == 0 && itr->second.Grabs == 0)
        {
            continue;
        }
        Row NewRow = { itr->first, itr->second };
        m_Rows.push_back(NewRow);
        TotalSeconds += itr->second.Seconds;
        TotalGrabs += itr->second.Grabs;
    }
    for (size_t i = 0; i < m_Rows.size(); i++)
    {
        // The castle is mostly walking between courses, it isn't a star
        if (m_Rows[i].Key / 8 != 0 && (MostTime == nullptr || m_Rows[i].Stats.Seconds > MostTime->Stats.Seconds))
        {
            MostTime = &m_Rows[i];
        }
    }

    std::wstring Summary;
    if (m_Rows.empty())
    {
        Summary = L"No star times yet. They are tracked for Super Mario 64 hacks built on the original game engine.";
    }
    else
    {
        Summary = L"Tracked: " + CPlayTime::FormatDuration(TotalSeconds) + stdstr_f(", %u star grab%s (practice grabs included).", TotalGrabs, TotalGrabs == 1 ? "" : "s").ToUTF16();
        if (MostTime != nullptr && MostTime->Stats.Seconds > 0)
        {
            Summary += L"\nMost time: " + CourseName(MostTime->Key / 8) + L", " + StarName(MostTime->Key) + L" (" + CPlayTime::FormatDuration(MostTime->Stats.Seconds) + L")";
        }
        Summary += L"\nTime is counted for the star picked on the star select screen.";
    }
    GetDlgItem(IDC_STARS_SUMMARY).SetWindowText(Summary.c_str());

    m_List.Attach(GetDlgItem(IDC_STARS_LIST));
    m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    CRect rcList;
    m_List.GetClientRect(&rcList);
    int Width = rcList.Width() - GetSystemMetrics(SM_CXVSCROLL);
    m_List.InsertColumn(Column_Course, L"Course", LVCFMT_LEFT, Width * 22 / 100);
    m_List.InsertColumn(Column_Star, L"Star", LVCFMT_LEFT, Width * 24 / 100);
    m_List.InsertColumn(Column_Time, L"Time", LVCFMT_LEFT, Width * 16 / 100);
    m_List.InsertColumn(Column_Grabs, L"Grabs", LVCFMT_LEFT, Width * 13 / 100);
    m_List.InsertColumn(Column_FirstGrab, L"First Grab After", LVCFMT_LEFT, Width * 25 / 100);

    for (size_t i = 0; i < m_Rows.size(); i++)
    {
        const Row & r = m_Rows[i];
        int Item = m_List.InsertItem(LVIF_TEXT | LVIF_PARAM, (int)i, CourseName(r.Key / 8).c_str(), 0, 0, 0, (LPARAM)i);
        m_List.SetItemText(Item, Column_Star, StarName(r.Key).c_str());
        m_List.SetItemText(Item, Column_Time, r.Stats.Seconds > 0 ? CPlayTime::FormatDuration(r.Stats.Seconds).c_str() : L"");
        m_List.SetItemText(Item, Column_Grabs, r.Stats.Grabs > 0 ? stdstr_f("%u", r.Stats.Grabs).ToUTF16().c_str() : L"");
        m_List.SetItemText(Item, Column_FirstGrab, r.Stats.Grabs > 0 ? CPlayTime::FormatDuration(r.Stats.FirstGrabAfter).c_str() : L"");
    }
    m_List.SortItems(CompareItems, (LPARAM)this);
    return TRUE;
}

LRESULT CStarTimesDlg::OnColumnClick(LPNMHDR pnmh)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
    if (pnmv->iSubItem == m_SortColumn)
    {
        m_SortAscending = !m_SortAscending;
    }
    else
    {
        m_SortColumn = pnmv->iSubItem;
        m_SortAscending = m_SortColumn == Column_Course || m_SortColumn == Column_Star;
    }
    m_List.SortItems(CompareItems, (LPARAM)this);
    return 0;
}

LRESULT CStarTimesDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    EndDialog(IDOK);
    return 0;
}

static int Compare(uint32_t a, uint32_t b)
{
    return a < b ? -1 : (a > b ? 1 : 0);
}

int CALLBACK CStarTimesDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CStarTimesDlg * _this = (CStarTimesDlg *)lParamSort;
    const Row & a = _this->m_Rows[lParam1];
    const Row & b = _this->m_Rows[lParam2];

    int Result = 0;
    switch (_this->m_SortColumn)
    {
    case Column_Course:
    case Column_Star: Result = Compare(a.Key, b.Key); break;
    case Column_Time: Result = Compare(a.Stats.Seconds, b.Stats.Seconds); break;
    case Column_Grabs: Result = Compare(a.Stats.Grabs, b.Stats.Grabs); break;
    case Column_FirstGrab: Result = Compare(a.Stats.Grabs > 0 ? a.Stats.FirstGrabAfter : 0, b.Stats.Grabs > 0 ? b.Stats.FirstGrabAfter : 0); break;
    }
    if (Result == 0)
    {
        Result = Compare(a.Key, b.Key);
    }
    return _this->m_SortAscending ? Result : -Result;
}
