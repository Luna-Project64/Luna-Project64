#include "stdafx.h"
#include "SaveStateInfo.h"
#include <Common/File.h>
#include <Common/IniFile.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/Plugin.h>
#ifdef _WIN32
#include <windows.h>
#endif

CriticalSection CSaveStateInfo::m_CS;
std::string CSaveStateInfo::m_LabelsFileName;
uint64_t CSaveStateInfo::m_LabelsFileTime = 0;
std::map<uint32_t, std::string> CSaveStateInfo::m_Labels;

CPath CSaveStateInfo::StateDirectory(void)
{
    CPath Dir(g_Settings->LoadStringVal(Directory_InstantSave).c_str(), "");
    if (g_Settings->LoadBool(Setting_UniqueSaveDir))
    {
        Dir.AppendDirectory(g_Settings->LoadStringVal(Game_UniqueSaveDir).c_str());
    }
#ifdef _WIN32
    Dir.NormalizePath(CPath(CPath::MODULE_DIRECTORY));
#endif
    return Dir;
}

CPath CSaveStateInfo::StateFile(uint32_t Slot)
{
    CPath File(StateDirectory());
    if (Slot != 0)
    {
        File.SetNameExtension(stdstr_f("%s.pj%d", g_Settings->LoadStringVal(Rdb_GoodName).c_str(), Slot).c_str());
    }
    else
    {
        File.SetNameExtension(stdstr_f("%s.pj", g_Settings->LoadStringVal(Rdb_GoodName).c_str()).c_str());
    }
    return File;
}

CPath CSaveStateInfo::ThumbnailFile(uint32_t Slot)
{
    CPath File(StateDirectory());
    File.AppendDirectory("Thumbnails");
    File.SetNameExtension(stdstr_f("%s.bmp", StateFile(Slot).GetNameExtension().c_str()).c_str());
    return File;
}

CPath CSaveStateInfo::LabelsFile(void)
{
    CPath File(StateDirectory());
    File.SetNameExtension(stdstr_f("%s.labels", g_Settings->LoadStringVal(Rdb_GoodName).c_str()).c_str());
    return File;
}

void CSaveStateInfo::LoadLabels(const CPath & File)
{
    // Called with m_CS held. Labels are read for every slot whenever the menu is
    // rebuilt, so keep them cached until the file changes on disk.
    uint64_t FileTime = 0;
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA Data;
    if (GetFileAttributesExA(File, GetFileExInfoStandard, &Data))
    {
        FileTime = ((uint64_t)Data.ftLastWriteTime.dwHighDateTime << 32) | Data.ftLastWriteTime.dwLowDateTime;
    }
#endif
    if (m_LabelsFileName == (const std::string &)File && m_LabelsFileTime == FileTime)
    {
        return;
    }
    m_LabelsFileName = (const std::string &)File;
    m_LabelsFileTime = FileTime;
    m_Labels.clear();
    if (FileTime == 0)
    {
        return;
    }

    CIniFile LabelsIni(File, false, true);
    CIniFileBase::strlist Keys;
    LabelsIni.GetKeyList("Labels", Keys);
    for (CIniFileBase::strlist::const_iterator itr = Keys.begin(); itr != Keys.end(); itr++)
    {
        uint32_t Slot;
        if (sscanf(itr->c_str(), "Slot %u", &Slot) == 1)
        {
            m_Labels[Slot] = LabelsIni.GetString("Labels", itr->c_str(), "");
        }
    }
}

std::string CSaveStateInfo::GetLabel(uint32_t Slot)
{
    CGuard Guard(m_CS);
    LoadLabels(LabelsFile());
    std::map<uint32_t, std::string>::const_iterator itr = m_Labels.find(Slot);
    return itr != m_Labels.end() ? itr->second : "";
}

void CSaveStateInfo::SetLabel(uint32_t Slot, const char * Label)
{
    CGuard Guard(m_CS);
    CPath File(LabelsFile());
    if (!File.DirectoryExists())
    {
        File.DirectoryCreate();
    }
    {
        CIniFile LabelsIni(File);
        stdstr Value(Label != nullptr ? Label : "");
        Value.Trim();
        LabelsIni.SaveString("Labels", stdstr_f("Slot %d", Slot).c_str(), Value.empty() ? nullptr : Value.c_str());
    }
    m_LabelsFileName.clear();
}


uint32_t CSaveStateInfo::CaptureMessage(void)
{
#ifdef _WIN32
    static UINT Message = RegisterWindowMessageW(L"LunaProject64.CaptureSaveStateThumbnail");
    return Message;
#else
    return 0;
#endif
}

void CSaveStateInfo::CaptureThumbnail(uint32_t Slot)
{
    CPath Thumbnail(ThumbnailFile(Slot));

    // Never leave a thumbnail of an older state behind
    if (Thumbnail.Exists())
    {
        Thumbnail.Delete();
    }

#ifdef _WIN32
    // PrintWindow sends messages to the window, so let the UI thread take the
    // screenshot rather than risk the emulation thread waiting on it
    RenderWindow * Window = g_Plugins != nullptr ? g_Plugins->MainWindow() : nullptr;
    HWND hMain = Window != nullptr ? (HWND)Window->GetWindowHandle() : nullptr;
    if (hMain != nullptr)
    {
        PostMessage(hMain, CaptureMessage(), Slot, 0);
    }
#endif
}

void CSaveStateInfo::WriteThumbnail(uint32_t Slot)
{
#ifdef _WIN32
    RenderWindow * Window = g_Plugins != nullptr ? g_Plugins->MainWindow() : nullptr;
    HWND hMain = Window != nullptr ? (HWND)Window->GetWindowHandle() : nullptr;
    if (hMain == nullptr || !IsWindowVisible(hMain) || IsIconic(hMain) || !g_Settings->LoadBool(GameRunning_CPU_Running))
    {
        return;
    }

    // The video plugin renders straight into the main window, above the status bar
    RECT rcClient;
    GetClientRect(hMain, &rcClient);
    int SrcWidth = rcClient.right, SrcHeight = rcClient.bottom;
    HWND hStatus = (HWND)Window->GetStatusBar();
    RECT rcStatus;
    if (hStatus != nullptr && IsWindowVisible(hStatus) && GetWindowRect(hStatus, &rcStatus))
    {
        SrcHeight -= rcStatus.bottom - rcStatus.top;
    }
    if (SrcWidth <= 0 || SrcHeight <= 0)
    {
        return;
    }

    int Width = ThumbnailWidth;
    int Height = (int)((int64_t)Width * SrcHeight / SrcWidth);
    if (Height <= 0 || Height > Width * 2)
    {
        return;
    }
    int Stride = (Width * 3 + 3) & ~3;

    BITMAPINFOHEADER bih = { 0 };
    bih.biSize = sizeof(bih);
    bih.biWidth = Width;
    bih.biHeight = Height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = Stride * Height;

    HDC hScreenDC = GetDC(nullptr);
    HDC hWindowDC = CreateCompatibleDC(hScreenDC);
    HDC hThumbDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hWindowBitmap = CreateCompatibleBitmap(hScreenDC, rcClient.right, rcClient.bottom);
    void * Bits = nullptr;
    HBITMAP hThumbBitmap = CreateDIBSection(hScreenDC, (BITMAPINFO *)&bih, DIB_RGB_COLORS, &Bits, nullptr, 0);
    bool Captured = false;
    if (hWindowDC != nullptr && hThumbDC != nullptr && hWindowBitmap != nullptr && hThumbBitmap != nullptr)
    {
        // PW_RENDERFULLCONTENT also captures DirectX/OpenGL content, even when other windows cover the game
        const UINT PrintFlags = PW_CLIENTONLY | 0x00000002 /* PW_RENDERFULLCONTENT */;
        HGDIOBJ hOldWindow = SelectObject(hWindowDC, hWindowBitmap);
        HGDIOBJ hOldThumb = SelectObject(hThumbDC, hThumbBitmap);
        if (PrintWindow(hMain, hWindowDC, PrintFlags))
        {
            SetStretchBltMode(hThumbDC, HALFTONE);
            SetBrushOrgEx(hThumbDC, 0, 0, nullptr);
            Captured = StretchBlt(hThumbDC, 0, 0, Width, Height, hWindowDC, 0, 0, SrcWidth, SrcHeight, SRCCOPY) != FALSE;
            GdiFlush();
        }
        SelectObject(hThumbDC, hOldThumb);
        SelectObject(hWindowDC, hOldWindow);
    }

    if (Captured && Bits != nullptr)
    {
        CPath Thumbnail(ThumbnailFile(Slot));
        if (!Thumbnail.DirectoryExists())
        {
            Thumbnail.DirectoryCreate();
        }
        BITMAPFILEHEADER bfh = { 0 };
        bfh.bfType = 0x4D42; // "BM"
        bfh.bfOffBits = sizeof(bfh) + sizeof(bih);
        bfh.bfSize = bfh.bfOffBits + bih.biSizeImage;

        CFile hFile(Thumbnail, CFileBase::modeWrite | CFileBase::modeCreate);
        if (hFile.IsOpen())
        {
            hFile.Write(&bfh, sizeof(bfh));
            hFile.Write(&bih, sizeof(bih));
            hFile.Write(Bits, bih.biSizeImage);
            hFile.Close();
        }
    }

    if (hThumbBitmap != nullptr)
    {
        DeleteObject(hThumbBitmap);
    }
    if (hWindowBitmap != nullptr)
    {
        DeleteObject(hWindowBitmap);
    }
    if (hThumbDC != nullptr)
    {
        DeleteDC(hThumbDC);
    }
    if (hWindowDC != nullptr)
    {
        DeleteDC(hWindowDC);
    }
    ReleaseDC(nullptr, hScreenDC);
#endif
}
