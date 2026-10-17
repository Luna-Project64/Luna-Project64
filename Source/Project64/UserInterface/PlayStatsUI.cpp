#include "stdafx.h"
#include "PlayStatsUI.h"
#include "StarTimesUI.h"

CPlayStatsDlg::CPlayStatsDlg(void) :
    m_SortColumn(Column_PlayTime),
    m_SortAscending(false)
{
}

LRESULT CPlayStatsDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    CPlayTime::GameStatsMap AllStats = CPlayTime::Get().GetAllStats();
    uint32_t TotalSeconds = 0, TotalSessions = 0;
    const CPlayTime::GameStats * MostPlayed = nullptr, * LongestSession = nullptr;
    for (CPlayTime::GameStatsMap::const_iterator itr = AllStats.begin(); itr != AllStats.end(); itr++)
    {
        const CPlayTime::GameStats & Stats = itr->second;
        if (Stats.Seconds == 0)
        {
            continue;
        }
        m_Games.push_back(Stats);
        m_Keys.push_back(itr->first);
        if (m_Games.back().Name.empty())
        {
            m_Games.back().Name = itr->first;
        }
        TotalSeconds += Stats.Seconds;
        TotalSessions += Stats.Sessions;
    }

    for (size_t i = 0; i < m_Games.size(); i++)
    {
        if (MostPlayed == nullptr || m_Games[i].Seconds > MostPlayed->Seconds)
        {
            MostPlayed = &m_Games[i];
        }
        if (LongestSession == nullptr || m_Games[i].LongestSession > LongestSession->LongestSession)
        {
            LongestSession = &m_Games[i];
        }
    }

    std::wstring Summary;
    if (m_Games.empty())
    {
        Summary = L"Nothing tracked yet. Play time is counted while a game is running and not paused.";
    }
    else
    {
        Summary = stdstr_f("Total play time: %s across %d game%s, %u session%s.",
            stdstr().FromUTF16(CPlayTime::FormatDuration(TotalSeconds).c_str()).c_str(),
            (int)m_Games.size(), m_Games.size() == 1 ? "" : "s",
            TotalSessions, TotalSessions == 1 ? "" : "s").ToUTF16();
        Summary += L"\nMost played: " + stdstr(MostPlayed->Name).ToUTF16() + L" (" + CPlayTime::FormatDuration(MostPlayed->Seconds) + L")";
        if (LongestSession->LongestSession > 0)
        {
            Summary += L"     Longest session: " + CPlayTime::FormatDuration(LongestSession->LongestSession) + L" (" + stdstr(LongestSession->Name).ToUTF16() + L")";
        }
    }
    GetDlgItem(IDC_STATS_SUMMARY).SetWindowText(Summary.c_str());

    m_List.Attach(GetDlgItem(IDC_STATS_LIST));
    m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    CRect rcList;
    m_List.GetClientRect(&rcList);
    int Width = rcList.Width() - GetSystemMetrics(SM_CXVSCROLL);
    m_List.InsertColumn(Column_Name, L"Game", LVCFMT_LEFT, Width * 40 / 100);
    m_List.InsertColumn(Column_PlayTime, L"Play Time", LVCFMT_LEFT, Width * 14 / 100);
    m_List.InsertColumn(Column_Sessions, L"Sessions", LVCFMT_LEFT, Width * 12 / 100);
    m_List.InsertColumn(Column_Longest, L"Longest Session", LVCFMT_LEFT, Width * 16 / 100);
    m_List.InsertColumn(Column_LastPlayed, L"Last Played", LVCFMT_LEFT, Width * 18 / 100);

    for (size_t i = 0; i < m_Games.size(); i++)
    {
        const CPlayTime::GameStats & Stats = m_Games[i];
        int Item = m_List.InsertItem(LVIF_TEXT | LVIF_PARAM, (int)i, stdstr(Stats.Name).ToUTF16().c_str(), 0, 0, 0, (LPARAM)i);
        m_List.SetItemText(Item, Column_PlayTime, CPlayTime::FormatDuration(Stats.Seconds).c_str());
        m_List.SetItemText(Item, Column_Sessions, stdstr_f("%u", Stats.Sessions).ToUTF16().c_str());
        m_List.SetItemText(Item, Column_Longest, Stats.LongestSession > 0 ? CPlayTime::FormatDuration(Stats.LongestSession).c_str() : L"");
        m_List.SetItemText(Item, Column_LastPlayed, CPlayTime::FormatDate(Stats.LastPlayed).c_str());
    }
    SortList();
    if (!m_Games.empty())
    {
        m_List.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    }
    UpdateStarButton();
    return TRUE;
}

int CPlayStatsDlg::SelectedGame(void)
{
    int Item = m_List.GetNextItem(-1, LVNI_SELECTED);
    return Item >= 0 ? (int)m_List.GetItemData(Item) : -1;
}

void CPlayStatsDlg::UpdateStarButton(void)
{
    int Game = SelectedGame();
    GetDlgItem(IDC_STATS_STARS).EnableWindow(Game >= 0 && !CPlayTime::Get().GetStarStats(m_Keys[Game]).empty());
}

void CPlayStatsDlg::ShowStarTimes(void)
{
    int Game = SelectedGame();
    if (Game >= 0 && !CPlayTime::Get().GetStarStats(m_Keys[Game]).empty())
    {
        CStarTimesDlg(m_Keys[Game], m_Games[Game].Name).DoModal(m_hWnd);
    }
}

LRESULT CPlayStatsDlg::OnItemChanged(LPNMHDR /*pnmh*/)
{
    UpdateStarButton();
    return 0;
}

LRESULT CPlayStatsDlg::OnDoubleClick(LPNMHDR /*pnmh*/)
{
    ShowStarTimes();
    return 0;
}

LRESULT CPlayStatsDlg::OnStarTimes(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    ShowStarTimes();
    return 0;
}

LRESULT CPlayStatsDlg::OnColumnClick(LPNMHDR pnmh)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
    if (pnmv->iSubItem == m_SortColumn)
    {
        m_SortAscending = !m_SortAscending;
    }
    else
    {
        m_SortColumn = pnmv->iSubItem;
        // Names read best A-Z, numbers and dates biggest/newest first
        m_SortAscending = m_SortColumn == Column_Name;
    }
    SortList();
    return 0;
}

LRESULT CPlayStatsDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    EndDialog(IDOK);
    return 0;
}

void CPlayStatsDlg::SortList(void)
{
    m_List.SortItems(CompareItems, (LPARAM)this);
}

int CALLBACK CPlayStatsDlg::CompareItems(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CPlayStatsDlg * _this = (CPlayStatsDlg *)lParamSort;
    const CPlayTime::GameStats & a = _this->m_Games[lParam1];
    const CPlayTime::GameStats & b = _this->m_Games[lParam2];

    int Result = 0;
    switch (_this->m_SortColumn)
    {
    case Column_Name: Result = _stricmp(a.Name.c_str(), b.Name.c_str()); break;
    case Column_PlayTime: Result = a.Seconds < b.Seconds ? -1 : (a.Seconds > b.Seconds ? 1 : 0); break;
    case Column_Sessions: Result = a.Sessions < b.Sessions ? -1 : (a.Sessions > b.Sessions ? 1 : 0); break;
    case Column_Longest: Result = a.LongestSession < b.LongestSession ? -1 : (a.LongestSession > b.LongestSession ? 1 : 0); break;
    case Column_LastPlayed: Result = a.LastPlayed < b.LastPlayed ? -1 : (a.LastPlayed > b.LastPlayed ? 1 : 0); break;
    }
    return _this->m_SortAscending ? Result : -Result;
}
