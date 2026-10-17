#pragma once
#include <Common/CriticalSection.h>
#include <stdint.h>
#include <string>
#include <vector>

// Reads a few Super Mario 64 globals from RDRAM so the UI can attribute play
// time to individual stars. Only hacks built on the vanilla US engine are
// supported (ROM Manager / Toad's Tool style hacks); decomp hacks move these
// variables around, so they are detected and ignored.

class CSm64Probe
{
public:
    enum
    {
        CourseCount = 26, // COURSE_NONE (castle) .. COURSE_CAKE_END
        StarsPerCourse = 7,
    };

    struct Snapshot
    {
        bool Valid;
        int16_t Course;
        int16_t Act;
        uint64_t TimeMs;
    };

    struct StarGrab
    {
        int16_t Course;
        int16_t Star; // 0 based
        int16_t Act;
    };

    // Emulation thread
    static void RomOpened(const uint8_t * Rom, uint32_t RomSize);
    static void StateLoading(void);
    static void OnViRefresh(const uint8_t * Rdram, uint32_t RdramSize);

    // Course and act names as the hack shows them (UTF-8). Course[0] (castle) is
    // always empty, Act[] holds the 6 star select names of courses 1..15.
    struct Names
    {
        std::string Course[CourseCount];
        std::string Act[16][6];
    };

    // Any thread
    static bool Supported(void);
    static Snapshot GetSnapshot(void);
    static std::vector<StarGrab> TakeStarGrabs(void);
    static bool GetNames(Names & Out);

    // Exposed for tests: decode SM64 text at Addr in RDRAM
    static std::string DecodeText(const uint8_t * Rdram, uint32_t RdramSize, uint32_t Addr);

private:
    enum
    {
        SampleEveryVi = 30,
    };

    static bool ReadStars(const uint8_t * Rdram, int16_t SaveFile, uint8_t (&Stars)[CourseCount]);
    static bool ReadNames(const uint8_t * Rdram, uint32_t RdramSize, Names & Out);

    static bool m_HaveNames;
    static Names m_Names;

    static CriticalSection m_CS;
    static bool m_Supported;
    static bool m_Resync;
    static bool m_HaveStars;
    static int16_t m_SaveFile;
    static uint8_t m_Stars[CourseCount];
    static uint32_t m_ViCount;
    static Snapshot m_Snapshot;
    static std::vector<StarGrab> m_Grabs;
};
