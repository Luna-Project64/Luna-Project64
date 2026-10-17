#include "stdafx.h"
#include "SaveStateManagerUI.h"
#include <Project64-core/N64System/SaveStateInfo.h>

#ifdef RETROACHIEVEMENTS
#include <Project64-core/RetroAchievements.h>
#endif

CSaveStateManagerDlg::CSaveStateManagerDlg(void) :
    m_Preview(nullptr),
    m_RefreshCount(0)
{
}

std::wstring CSaveStateManagerDlg::SlotName(int Slot)
{
    return Slot == 0 ? L"Default" : stdstr_f("Slot %d", Slot).ToUTF16();
}

bool CSaveStateManagerDlg::GetSavedTime(int Slot, std::wstring & Time)
{
    CPath File(CSaveStateInfo::StateFile(Slot));
    CPath ZipFile(File.GetDriveDirectory().c_str(), stdstr_f("%s.zip", File.GetNameExtension().c_str()).c_str());
    const CPath * Candidates[] = { &ZipFile, &File };

    for (size_t i = 0; i < sizeof(Candidates) / sizeof(Candidates[0]); i++)
    {
        WIN32_FILE_ATTRIBUTE_DATA Data;
        if (!GetFileAttributesExA(*Candidates[i], GetFileExInfoStandard, &Data))
        {
            continue;
        }
        SYSTEMTIME stUTC, stLocal;
        FileTimeToSystemTime(&Data.ftLastWriteTime, &stUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);
        Time = stdstr_f("%d-%02d-%02d %02d:%02d", stLocal.wYear, stLocal.wMonth, stLocal.wDay, stLocal.wHour, stLocal.wMinute).ToUTF16();
        return true;
    }
    Time.clear();
    return false;
}

LRESULT CSaveStateManagerDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    std::string GameName = g_Settings->LoadStringVal(Rdb_GoodName);
    if (!GameName.empty())
    {
        SetWindowText(stdstr_f("Savestate Manager - %s", GameName.c_str()).ToUTF16().c_str());
    }

    m_List.Attach(GetDlgItem(IDC_SS_LIST));
    m_List.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);

    CRect rcList;
    m_List.GetClientRect(&rcList);
    int Width = rcList.Width() - GetSystemMetrics(SM_CXVSCROLL);
    m_List.InsertColumn(Column_Slot, L"Slot", LVCFMT_LEFT, Width * 22 / 100);
    m_List.InsertColumn(Column_Label, L"Label", LVCFMT_LEFT, Width * 43 / 100);
    m_List.InsertColumn(Column_Saved, L"Saved", LVCFMT_LEFT, Width * 35 / 100);

    for (int Slot = 0; Slot < SlotCount; Slot++)
    {
        m_List.InsertItem(Slot, SlotName(Slot).c_str());
        RefreshSlot(Slot);
    }

    GetDlgItem(IDC_SS_LABEL).SendMessage(EM_LIMITTEXT, 100, 0);

    int CurrentSlot = (int)g_Settings->LoadDword(Game_CurrentSaveState);
    if (CurrentSlot < 0 || CurrentSlot >= SlotCount)
    {
        CurrentSlot = 0;
    }
    m_List.SetItemState(CurrentSlot, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
    m_List.EnsureVisible(CurrentSlot, FALSE);
    ShowSlotDetails(CurrentSlot);

    PlaceBesideMainWindow();
    m_List.SetFocus();
    return FALSE;
}

void CSaveStateManagerDlg::PlaceBesideMainWindow(void)
{
    // Keep the game visible next to this window
    HWND hMain = GetParent();
    if (hMain == nullptr)
    {
        return;
    }
    CRect rcMain, rcDlg;
    ::GetWindowRect(hMain, &rcMain);
    GetWindowRect(&rcDlg);

    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(MonitorFromWindow(hMain, MONITOR_DEFAULTTONEAREST), &mi))
    {
        return;
    }
    const RECT & Work = mi.rcWork;

    int X;
    if (rcMain.right + rcDlg.Width() <= Work.right)
    {
        X = rcMain.right;
    }
    else if (rcMain.left - rcDlg.Width() >= Work.left)
    {
        X = rcMain.left - rcDlg.Width();
    }
    else
    {
        return;
    }
    int Y = rcMain.top;
    if (Y + rcDlg.Height() > Work.bottom)
    {
        Y = Work.bottom - rcDlg.Height();
    }
    if (Y < Work.top)
    {
        Y = Work.top;
    }
    SetWindowPos(nullptr, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

void CSaveStateManagerDlg::RefreshSlot(int Slot)
{
    std::wstring Saved;
    GetSavedTime(Slot, Saved);
    m_List.SetItemText(Slot, Column_Label, stdstr(CSaveStateInfo::GetLabel(Slot)).ToUTF16().c_str());
    m_List.SetItemText(Slot, Column_Saved, Saved.empty() ? L"(empty)" : Saved.c_str());
}

int CSaveStateManagerDlg::SelectedSlot(void)
{
    return m_List.GetNextItem(-1, LVNI_SELECTED);
}

HBITMAP CSaveStateManagerDlg::ScaleToPreview(HBITMAP hSource)
{
    // Fit the thumbnail inside the preview frame, keeping its aspect ratio
    BITMAP bm;
    CRect rcPreview;
    GetDlgItem(IDC_SS_PREVIEW).GetClientRect(&rcPreview);
    if (GetObject(hSource, sizeof(bm), &bm) == 0 || bm.bmWidth <= 0 || bm.bmHeight <= 0 || rcPreview.Width() <= 4 || rcPreview.Height() <= 4)
    {
        return nullptr;
    }
    int MaxWidth = rcPreview.Width() - 4, MaxHeight = rcPreview.Height() - 4;
    int Width = MaxWidth, Height = (int)((int64_t)bm.bmHeight * MaxWidth / bm.bmWidth);
    if (Height > MaxHeight)
    {
        Height = MaxHeight;
        Width = (int)((int64_t)bm.bmWidth * MaxHeight / bm.bmHeight);
    }

    HDC hScreenDC = ::GetDC(nullptr);
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HDC hDstDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hScaled = CreateCompatibleBitmap(hScreenDC, Width, Height);
    if (hSrcDC != nullptr && hDstDC != nullptr && hScaled != nullptr)
    {
        HGDIOBJ hOldSrc = SelectObject(hSrcDC, hSource);
        HGDIOBJ hOldDst = SelectObject(hDstDC, hScaled);
        SetStretchBltMode(hDstDC, HALFTONE);
        SetBrushOrgEx(hDstDC, 0, 0, nullptr);
        StretchBlt(hDstDC, 0, 0, Width, Height, hSrcDC, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
        SelectObject(hDstDC, hOldDst);
        SelectObject(hSrcDC, hOldSrc);
    }
    if (hDstDC != nullptr)
    {
        DeleteDC(hDstDC);
    }
    if (hSrcDC != nullptr)
    {
        DeleteDC(hSrcDC);
    }
    ::ReleaseDC(nullptr, hScreenDC);
    return hScaled;
}

void CSaveStateManagerDlg::ShowSlotDetails(int Slot)
{
    if (m_Preview != nullptr)
    {
        GetDlgItem(IDC_SS_PREVIEW).SendMessage(STM_SETIMAGE, IMAGE_BITMAP, 0);
        DeleteObject(m_Preview);
        m_Preview = nullptr;
    }

    std::wstring Saved;
    bool HasState = Slot >= 0 && GetSavedTime(Slot, Saved);
    if (HasState)
    {
        CPath Thumbnail(CSaveStateInfo::ThumbnailFile(Slot));
        HBITMAP hThumbnail = (HBITMAP)LoadImage(nullptr, stdstr((const std::string &)Thumbnail).ToUTF16().c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
        if (hThumbnail != nullptr)
        {
            m_Preview = ScaleToPreview(hThumbnail);
            DeleteObject(hThumbnail);
        }
    }
    if (m_Preview != nullptr)
    {
        GetDlgItem(IDC_SS_PREVIEW).SendMessage(STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)m_Preview);
    }
    GetDlgItem(IDC_SS_NO_PREVIEW).SetWindowText(HasState ? L"No thumbnail for this slot" : L"This slot is empty");
    // The text sits on top of the preview frame, so repaint the frame first
    GetDlgItem(IDC_SS_PREVIEW).Invalidate();
    GetDlgItem(IDC_SS_PREVIEW).UpdateWindow();
    GetDlgItem(IDC_SS_NO_PREVIEW).ShowWindow(m_Preview != nullptr ? SW_HIDE : SW_SHOW);
    GetDlgItem(IDC_SS_NO_PREVIEW).Invalidate();

    GetDlgItem(IDC_SS_LABEL).SetWindowText(Slot >= 0 ? stdstr(CSaveStateInfo::GetLabel(Slot)).ToUTF16().c_str() : L"");
    GetDlgItem(IDC_SS_LABEL).EnableWindow(Slot >= 0);
    GetDlgItem(IDC_SS_SET_LABEL).EnableWindow(Slot >= 0);
    GetDlgItem(IDC_SS_LOAD).EnableWindow(HasState);
    GetDlgItem(IDC_SS_SAVE).EnableWindow(Slot >= 0);
}

LRESULT CSaveStateManagerDlg::OnItemChanged(LPNMHDR pnmh)
{
    LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
    if ((pnmv->uChanged & LVIF_STATE) != 0 && ((pnmv->uNewState ^ pnmv->uOldState) & LVIS_SELECTED) != 0)
    {
        ShowSlotDetails(SelectedSlot());
    }
    return 0;
}

LRESULT CSaveStateManagerDlg::OnDoubleClick(LPNMHDR pnmh)
{
    LPNMITEMACTIVATE pnmia = (LPNMITEMACTIVATE)pnmh;
    std::wstring Saved;
    if (pnmia->iItem >= 0 && GetSavedTime(pnmia->iItem, Saved))
    {
        LoadSlot(pnmia->iItem);
    }
    return 0;
}

LRESULT CSaveStateManagerDlg::OnSetLabel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    int Slot = SelectedSlot();
    if (Slot < 0)
    {
        return 0;
    }
    wchar_t Label[128] = { 0 };
    GetDlgItemText(IDC_SS_LABEL, Label, sizeof(Label) / sizeof(Label[0]));
    CSaveStateInfo::SetLabel(Slot, stdstr().FromUTF16(Label).c_str());
    RefreshSlot(Slot);
    return 0;
}

void CSaveStateManagerDlg::LoadSlot(int Slot)
{
#ifdef RETROACHIEVEMENTS
    if (!RA_WarnDisableHardcore("load a state"))
    {
        return;
    }
#endif
    g_Settings->SaveDword(Game_CurrentSaveState, (uint32_t)Slot);
    g_BaseSystem->ExternalEvent(SysEvent_LoadMachineState);
    EndDialog(IDOK);
}

LRESULT CSaveStateManagerDlg::OnLoad(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    int Slot = SelectedSlot();
    if (Slot >= 0 && g_BaseSystem != nullptr)
    {
        LoadSlot(Slot);
    }
    return 0;
}

LRESULT CSaveStateManagerDlg::OnSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    int Slot = SelectedSlot();
    if (Slot < 0 || g_BaseSystem == nullptr)
    {
        return 0;
    }
    std::wstring Saved;
    if (GetSavedTime(Slot, Saved))
    {
        std::wstring Question = L"Overwrite the state saved in " + SlotName(Slot) + L" (" + Saved + L")?";
        if (MessageBox(Question.c_str(), L"Savestate Manager", MB_YESNO | MB_ICONQUESTION) != IDYES)
        {
            return 0;
        }
    }
    g_Settings->SaveDword(Game_CurrentSaveState, (uint32_t)Slot);
    g_BaseSystem->ExternalEvent(SysEvent_SaveMachineState);

    // The state is written by the emulation thread, refresh once it had time to finish
    m_RefreshCount = 0;
    SetTimer(Timer_Refresh, 500);
    return 0;
}

LRESULT CSaveStateManagerDlg::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    if (wParam == Timer_Refresh)
    {
        int Slot = SelectedSlot();
        if (Slot >= 0)
        {
            RefreshSlot(Slot);
            ShowSlotDetails(Slot);
        }
        if (++m_RefreshCount >= 3)
        {
            KillTimer(Timer_Refresh);
        }
    }
    return 0;
}

LRESULT CSaveStateManagerDlg::OnClose(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL & /*bHandled*/)
{
    EndDialog(IDCANCEL);
    return 0;
}

LRESULT CSaveStateManagerDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & bHandled)
{
    KillTimer(Timer_Refresh);
    if (m_Preview != nullptr)
    {
        DeleteObject(m_Preview);
        m_Preview = nullptr;
    }
    bHandled = FALSE;
    return 0;
}
