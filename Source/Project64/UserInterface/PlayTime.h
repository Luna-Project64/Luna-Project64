#pragma once
#include <Common/CriticalSection.h>
#include <map>
#include <string>
#include <vector>

// Tracks how long each game has been played. Time only counts while the CPU is
// running and not paused. Games are identified the same way as the ROM database
// does it (CRC1-CRC2-C:Country), so renaming or moving a ROM keeps its stats.
// Stats are stored in PlayTime.ini next to Project64.cfg.

class CPlayTime
{
public:
    struct GameStats
    {
        std::string Name;
        uint32_t Seconds;
        uint32_t Sessions;
        uint32_t LongestSession;
        uint32_t FirstPlayed;
        uint32_t LastPlayed;
    };
    typedef std::map<std::string, GameStats> GameStatsMap;

    // SM64 hacks only: time spent per star (the act selected on the star select
    // screen, including practice and savestate loads) and how often it was grabbed
    enum
    {
        StarAnyAct = 7, // Time in a course without a star select (castle, bowser, secret courses)
    };
    struct StarStats
    {
        uint32_t Seconds;
        uint32_t Grabs;
        uint32_t FirstGrabAfter; // Seconds spent on the star before the first grab, valid if Grabs > 0
    };
    typedef std::map<int, StarStats> StarStatsMap; // Key: Course * 8 + Star (0 based) or StarAnyAct
    static int StarKey(int Course, int Star) { return Course * 8 + Star; }

    StarStatsMap GetStarStats(const std::string & Key);

    static CPlayTime & Get(void);

    void Start(void);
    void Stop(void);

    bool GetStats(const std::string & Key, GameStats & Stats);
    GameStatsMap GetAllStats(void);

    static std::string RomKey(uint32_t CRC1, uint32_t CRC2, uint8_t Country);
    static std::wstring FormatDuration(uint32_t Seconds);
    static std::wstring FormatDate(uint32_t Time);

private:
    enum
    {
        MinSessionSeconds = 10,
        FlushIntervalSeconds = 60,
    };

    CPlayTime(void);
    CPlayTime(const CPlayTime &);
    CPlayTime & operator=(const CPlayTime &);

    static DWORD WINAPI stThreadProc(LPVOID Param);
    void ThreadProc(void);
    void BeginSession(void);
    void EndSession(void);
    void Flush(void);
    void TrackStars(uint32_t Seconds);
    std::string IniFileName(const char * FileName);

    CriticalSection m_CS;
    GameStatsMap m_Stats;
    std::map<std::string, StarStatsMap> m_StarStats;
    HANDLE m_Thread;
    HANDLE m_StopEvent;

    // Current session, only touched while holding m_CS
    bool m_InSession;
    std::string m_SessionKey;
    uint32_t m_SessionSeconds;
    uint32_t m_UnflushedSeconds;
};
