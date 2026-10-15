#pragma once
#include <Common/CriticalSection.h>
#include <stdint.h>
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

    // Any thread
    static bool Supported(void);
    static Snapshot GetSnapshot(void);
    static std::vector<StarGrab> TakeStarGrabs(void);

private:
    enum
    {
        SampleEveryVi = 30,
    };

    static bool ReadStars(const uint8_t * Rdram, int16_t SaveFile, uint8_t (&Stars)[CourseCount]);

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
