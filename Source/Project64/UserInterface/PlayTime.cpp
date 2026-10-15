#include "stdafx.h"
#include "PlayTime.h"
#include <Common/IniFile.h>
#include <time.h>

CPlayTime & CPlayTime::Get(void)
{
    static CPlayTime PlayTime;
    return PlayTime;
}

CPlayTime::CPlayTime(void) :
    m_Thread(nullptr),
    m_StopEvent(nullptr),
    m_InSession(false),
    m_SessionSeconds(0),
    m_UnflushedSeconds(0)
{
}

std::string CPlayTime::IniFileName(void)
{
    CPath IniFile(CPath(g_Settings->LoadStringVal(SupportFile_Settings).c_str()).GetDriveDirectory().c_str(), "PlayTime.ini");
    return (const std::string &)IniFile;
}

static uint32_t IniGetUInt(CIniFile & Ini, const char * Section, const char * Key)
{
    return strtoul(Ini.GetString(Section, Key, "0").c_str(), nullptr, 10);
}

void CPlayTime::Start(void)
{
    if (m_Thread != nullptr)
    {
        return;
    }

    {
        CGuard Guard(m_CS);
        m_Stats.clear();
        CIniFile Ini(IniFileName().c_str(), false, true);
        std::vector<std::string> Sections = Ini.GetVectorOfSectionsSorted();
        for (size_t i = 0; i < Sections.size(); i++)
        {
            const char * Section = Sections[i].c_str();
            GameStats & Stats = m_Stats[Sections[i]];
            Stats.Name = Ini.GetString(Section, "Name", "");
            Stats.Seconds = IniGetUInt(Ini, Section, "Play Time");
            Stats.Sessions = IniGetUInt(Ini, Section, "Sessions");
            Stats.LongestSession = IniGetUInt(Ini, Section, "Longest Session");
            Stats.FirstPlayed = IniGetUInt(Ini, Section, "First Played");
            Stats.LastPlayed = IniGetUInt(Ini, Section, "Last Played");
        }
    }

    m_StopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    m_Thread = CreateThread(nullptr, 0, stThreadProc, this, 0, nullptr);
}

void CPlayTime::Stop(void)
{
    if (m_Thread == nullptr)
    {
        return;
    }
    SetEvent(m_StopEvent);
    WaitForSingleObject(m_Thread, INFINITE);
    CloseHandle(m_Thread);
    CloseHandle(m_StopEvent);
    m_Thread = nullptr;
    m_StopEvent = nullptr;

    CGuard Guard(m_CS);
    EndSession();
}

bool CPlayTime::GetStats(const std::string & Key, GameStats & Stats)
{
    CGuard Guard(m_CS);
    GameStatsMap::const_iterator itr = m_Stats.find(Key);
    if (itr == m_Stats.end())
    {
        return false;
    }
    Stats = itr->second;
    return true;
}

CPlayTime::GameStatsMap CPlayTime::GetAllStats(void)
{
    CGuard Guard(m_CS);
    return m_Stats;
}

std::string CPlayTime::RomKey(uint32_t CRC1, uint32_t CRC2, uint8_t Country)
{
    return stdstr_f("%08X-%08X-C:%X", CRC1, CRC2, Country);
}

std::wstring CPlayTime::FormatDuration(uint32_t Seconds)
{
    uint32_t Minutes = Seconds / 60;
    if (Minutes == 0)
    {
        return Seconds == 0 ? L"0m" : L"< 1m";
    }
    if (Minutes < 60)
    {
        return stdstr_f("%um", Minutes).ToUTF16();
    }
    return stdstr_f("%uh %02um", Minutes / 60, Minutes % 60).ToUTF16();
}

std::wstring CPlayTime::FormatDate(uint32_t Time)
{
    if (Time == 0)
    {
        return L"";
    }
    time_t t = (time_t)Time;
    struct tm Local;
    if (localtime_s(&Local, &t) != 0)
    {
        return L"";
    }
    wchar_t Buffer[64];
    wcsftime(Buffer, sizeof(Buffer) / sizeof(Buffer[0]), L"%Y-%m-%d %H:%M", &Local);
    return Buffer;
}

DWORD WINAPI CPlayTime::stThreadProc(LPVOID Param)
{
    ((CPlayTime *)Param)->ThreadProc();
    return 0;
}

void CPlayTime::ThreadProc(void)
{
    ULONGLONG LastTick = GetTickCount64();
    ULONGLONG CarryMs = 0;

    while (WaitForSingleObject(m_StopEvent, 1000) == WAIT_TIMEOUT)
    {
        ULONGLONG Now = GetTickCount64();
        ULONGLONG Elapsed = Now - LastTick;
        LastTick = Now;
        if (Elapsed > 5000)
        {
            // The machine was probably asleep, don't count that
            Elapsed = 1000;
        }

        bool Running = g_Settings->LoadBool(GameRunning_CPU_Running) && !g_Settings->LoadBool(GameRunning_LoadingInProgress);
        bool Paused = g_Settings->LoadBool(GameRunning_CPU_Paused);
        std::string Key = Running ? g_Settings->LoadStringVal(Game_IniKey) : "";

        CGuard Guard(m_CS);
        if (m_InSession && (Key.empty() || Key != m_SessionKey))
        {
            EndSession();
        }
        if (Key.empty())
        {
            CarryMs = 0;
            continue;
        }
        if (!m_InSession)
        {
            BeginSession();
        }
        if (Paused)
        {
            continue;
        }

        CarryMs += Elapsed;
        uint32_t Seconds = (uint32_t)(CarryMs / 1000);
        CarryMs %= 1000;
        if (Seconds == 0)
        {
            continue;
        }

        uint32_t TimeNow = (uint32_t)time(nullptr);
        GameStatsMap::iterator itr = m_Stats.find(m_SessionKey);
        if (itr == m_Stats.end())
        {
            GameStats NewStats = { "", 0, 0, 0, TimeNow, TimeNow };
            itr = m_Stats.insert(GameStatsMap::value_type(m_SessionKey, NewStats)).first;
        }

        // Refresh the name in case the ROM database or file name changed
        std::string GoodName = g_Settings->LoadStringVal(Rdb_GoodName);
        if (GoodName.empty() || GoodName == g_Settings->LoadStringVal(Game_GameName))
        {
            // Not in the ROM database (most hacks), the file name is more useful than the internal name
            std::string FileName = CPath(g_Settings->LoadStringVal(Game_File)).GetName();
            if (!FileName.empty())
            {
                GoodName = FileName;
            }
        }
        if (!GoodName.empty())
        {
            itr->second.Name = GoodName;
        }
        if (itr->second.FirstPlayed == 0)
        {
            itr->second.FirstPlayed = TimeNow;
        }
        itr->second.Seconds += Seconds;
        itr->second.LastPlayed = TimeNow;
        m_SessionSeconds += Seconds;
        m_UnflushedSeconds += Seconds;
        if (m_UnflushedSeconds >= FlushIntervalSeconds)
        {
            Flush();
        }
    }
}

void CPlayTime::BeginSession(void)
{
    m_InSession = true;
    m_SessionKey = g_Settings->LoadStringVal(Game_IniKey);
    m_SessionSeconds = 0;
    m_UnflushedSeconds = 0;
}

void CPlayTime::EndSession(void)
{
    if (!m_InSession)
    {
        return;
    }
    GameStatsMap::iterator itr = m_Stats.find(m_SessionKey);
    if (itr != m_Stats.end() && m_SessionSeconds >= MinSessionSeconds)
    {
        itr->second.Sessions += 1;
        if (m_SessionSeconds > itr->second.LongestSession)
        {
            itr->second.LongestSession = m_SessionSeconds;
        }
    }
    Flush();
    m_InSession = false;
    m_SessionKey.clear();
    m_SessionSeconds = 0;
}

void CPlayTime::Flush(void)
{
    m_UnflushedSeconds = 0;
    GameStatsMap::const_iterator itr = m_Stats.find(m_SessionKey);
    if (itr == m_Stats.end())
    {
        return;
    }

    const char * Section = itr->first.c_str();
    const GameStats & Stats = itr->second;
    CIniFile Ini(IniFileName().c_str());
    Ini.SetAutoFlush(false);
    Ini.SaveString(Section, "Name", Stats.Name.c_str());
    Ini.SaveString(Section, "Play Time", stdstr_f("%u", Stats.Seconds).c_str());
    Ini.SaveString(Section, "Sessions", stdstr_f("%u", Stats.Sessions).c_str());
    Ini.SaveString(Section, "Longest Session", stdstr_f("%u", Stats.LongestSession).c_str());
    Ini.SaveString(Section, "First Played", stdstr_f("%u", Stats.FirstPlayed).c_str());
    Ini.SaveString(Section, "Last Played", stdstr_f("%u", Stats.LastPlayed).c_str());
    Ini.FlushChanges();
}
