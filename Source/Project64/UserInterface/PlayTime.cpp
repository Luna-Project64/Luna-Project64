#include "stdafx.h"
#include "PlayTime.h"
#include <Common/IniFile.h>
#include <Project64-core/N64System/Sm64Probe.h>
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

std::string CPlayTime::IniFileName(const char * FileName)
{
    CPath IniFile(CPath(g_Settings->LoadStringVal(SupportFile_Settings).c_str()).GetDriveDirectory().c_str(), FileName);
    return (const std::string &)IniFile;
}

static std::string StarIniKey(int Key)
{
    int Course = Key / 8, Star = Key % 8;
    return Star == CPlayTime::StarAnyAct ? stdstr_f("C%02d Any", Course) : stdstr_f("C%02d S%d", Course, Star + 1);
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
        m_StarStats.clear();
        CIniFile StarIni(IniFileName("StarTime.ini").c_str(), false, true);
        std::vector<std::string> StarSections = StarIni.GetVectorOfSectionsSorted();
        for (size_t i = 0; i < StarSections.size(); i++)
        {
            CIniFileBase::KeyValueData Data;
            StarIni.GetKeyValueData(StarSections[i].c_str(), Data);
            StarStatsMap & Stars = m_StarStats[StarSections[i]];
            for (CIniFileBase::KeyValueData::const_iterator itr = Data.begin(); itr != Data.end(); itr++)
            {
                int Course = 0, Star = 0;
                StarStats Stats = { 0, 0, 0 };
                if (sscanf(itr->second.c_str(), "%u,%u,%u", &Stats.Seconds, &Stats.Grabs, &Stats.FirstGrabAfter) != 3)
                {
                    continue;
                }
                if (sscanf(itr->first.c_str(), "C%d S%d", &Course, &Star) == 2 && Star >= 1 && Star <= 7)
                {
                    Stars[StarKey(Course, Star - 1)] = Stats;
                }
                else if (sscanf(itr->first.c_str(), "C%d Any", &Course) == 1)
                {
                    Stars[StarKey(Course, StarAnyAct)] = Stats;
                }
            }
        }

        CIniFile Ini(IniFileName("PlayTime.ini").c_str(), false, true);
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

CPlayTime::StarStatsMap CPlayTime::GetStarStats(const std::string & Key)
{
    CGuard Guard(m_CS);
    std::map<std::string, StarStatsMap>::const_iterator itr = m_StarStats.find(Key);
    return itr != m_StarStats.end() ? itr->second : StarStatsMap();
}

void CPlayTime::TrackStars(uint32_t Seconds)
{
    // Called with m_CS held, after Seconds were added to the current session
    if (!CSm64Probe::Supported())
    {
        return;
    }
    StarStatsMap & Stars = m_StarStats[m_SessionKey];

    // Time goes to the act picked on the star select screen
    int TimeKey = -1;
    CSm64Probe::Snapshot Snapshot = CSm64Probe::GetSnapshot();
    if (Snapshot.Valid && GetTickCount64() - Snapshot.TimeMs < 2000)
    {
        bool MainCourse = Snapshot.Course >= 1 && Snapshot.Course <= 15;
        bool ValidAct = Snapshot.Act >= 1 && Snapshot.Act <= 6;
        // Entering a main course briefly reports no act, that moment isn't worth its own row
        if (!MainCourse || ValidAct)
        {
            TimeKey = StarKey(Snapshot.Course, MainCourse ? Snapshot.Act - 1 : StarAnyAct);
            StarStats & Stats = Stars[TimeKey];
            Stats.Seconds += Seconds;
        }
    }

    std::vector<CSm64Probe::StarGrab> Grabs = CSm64Probe::TakeStarGrabs();
    for (size_t i = 0; i < Grabs.size(); i++)
    {
        const CSm64Probe::StarGrab & Grab = Grabs[i];
        StarStats & Stats = Stars[StarKey(Grab.Course, Grab.Star)];
        if (Stats.Grabs == 0)
        {
            // For stars without their own act (100 coins, secret stars) use the time of the act they were grabbed in
            bool MainCourse = Grab.Course >= 1 && Grab.Course <= 15 && Grab.Act >= 1 && Grab.Act <= 6;
            int ContextKey = Grab.Course >= 1 && Grab.Course <= 15 && Grab.Star < 6 ? StarKey(Grab.Course, Grab.Star) : StarKey(Grab.Course, MainCourse ? Grab.Act - 1 : StarAnyAct);
            StarStatsMap::const_iterator Context = Stars.find(ContextKey);
            Stats.FirstGrabAfter = Context != Stars.end() ? Context->second.Seconds : 0;
        }
        Stats.Grabs += 1;
    }
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
        TrackStars(Seconds);
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
    std::map<std::string, StarStatsMap>::const_iterator Stars = m_StarStats.find(m_SessionKey);
    if (Stars != m_StarStats.end() && !Stars->second.empty())
    {
        CIniFile StarIni(IniFileName("StarTime.ini").c_str());
        StarIni.SetAutoFlush(false);
        for (StarStatsMap::const_iterator Star = Stars->second.begin(); Star != Stars->second.end(); Star++)
        {
            const StarStats & s = Star->second;
            StarIni.SaveString(Section, StarIniKey(Star->first).c_str(), stdstr_f("%u,%u,%u", s.Seconds, s.Grabs, s.FirstGrabAfter).c_str());
        }
        StarIni.FlushChanges();
    }

    CIniFile Ini(IniFileName("PlayTime.ini").c_str());
    Ini.SetAutoFlush(false);
    Ini.SaveString(Section, "Name", Stats.Name.c_str());
    Ini.SaveString(Section, "Play Time", stdstr_f("%u", Stats.Seconds).c_str());
    Ini.SaveString(Section, "Sessions", stdstr_f("%u", Stats.Sessions).c_str());
    Ini.SaveString(Section, "Longest Session", stdstr_f("%u", Stats.LongestSession).c_str());
    Ini.SaveString(Section, "First Played", stdstr_f("%u", Stats.FirstPlayed).c_str());
    Ini.SaveString(Section, "Last Played", stdstr_f("%u", Stats.LastPlayed).c_str());
    Ini.FlushChanges();
}
