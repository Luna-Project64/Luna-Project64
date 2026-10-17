#include "stdafx.h"
#include "Sm64Probe.h"
#ifdef _WIN32
#include <windows.h>
#endif

// Vanilla SM64 (US) addresses
static const uint32_t SM64_gCurrSaveFileNum = 0x0032DDF4; // s16, 1 based
static const uint32_t SM64_gCurrCourseNum = 0x0033BAC6;   // s16
static const uint32_t SM64_gCurrActNum = 0x0033BAC8;      // s16
static const uint32_t SM64_gSaveBuffer = 0x00207700;      // struct SaveFile files[4][2]
static const uint32_t SM64_SaveFileSize = 0x38;
static const uint32_t SM64_SaveFileFlags = 0x08;          // u32, castle stars in bits 24..30
static const uint32_t SM64_SaveFileCourseStars = 0x0C;    // u8[25], bit 7 is the cannon
static const uint32_t SM64_sSegmentTable = 0x0033B400;    // u32[32], physical addresses
static const uint32_t SM64_CourseNameTable = 0x00010F68;  // in segment 2, 25 pointers (courses 1..25)
static const uint32_t SM64_ActNameTable = 0x0001192C;     // in segment 2, 15 * 6 pointers

CriticalSection CSm64Probe::m_CS;
bool CSm64Probe::m_Supported = false;
bool CSm64Probe::m_Resync = true;
bool CSm64Probe::m_HaveStars = false;
int16_t CSm64Probe::m_SaveFile = 0;
uint8_t CSm64Probe::m_Stars[CSm64Probe::CourseCount];
uint32_t CSm64Probe::m_ViCount = 0;
CSm64Probe::Snapshot CSm64Probe::m_Snapshot = { false, 0, 0, 0 };
std::vector<CSm64Probe::StarGrab> CSm64Probe::m_Grabs;
bool CSm64Probe::m_HaveNames = false;
CSm64Probe::Names CSm64Probe::m_Names;

// RDRAM is stored as native 32 bit words
static inline uint8_t ReadU8(const uint8_t * Rdram, uint32_t Addr) { return Rdram[Addr ^ 3]; }
static inline int16_t ReadS16(const uint8_t * Rdram, uint32_t Addr) { return *(const int16_t *)(Rdram + (Addr ^ 2)); }
static inline uint32_t ReadU32(const uint8_t * Rdram, uint32_t Addr) { return *(const uint32_t *)(Rdram + Addr); }

std::string CSm64Probe::DecodeText(const uint8_t * Rdram, uint32_t RdramSize, uint32_t Addr)
{
    // SM64 (US) character set, as used by course and act names
    std::string Text;
    for (uint32_t i = 0; i < 100 && Addr + i < RdramSize; i++)
    {
        uint8_t c = ReadU8(Rdram, Addr + i);
        if (c == 0xFF) { return Text; }
        if (c <= 0x09) { Text += (char)('0' + c); }
        else if (c <= 0x23) { Text += (char)('A' + c - 0x0A); }
        else if (c <= 0x3D) { Text += (char)('a' + c - 0x24); }
        else
        {
            switch (c)
            {
            case 0x3E: Text += '\''; break;
            case 0x3F: Text += '.'; break;
            case 0x6F: Text += ','; break;
            case 0x9E: Text += ' '; break;
            case 0x9F: Text += '-'; break;
            case 0xE1: Text += '('; break;
            case 0xE3: Text += ')'; break;
            case 0xE4: Text += '+'; break;
            case 0xE5: Text += '&'; break;
            case 0xE6: Text += ':'; break;
            case 0xF2: Text += '!'; break;
            case 0xF3: Text += '%'; break;
            case 0xF4: Text += '?'; break;
            case 0xF5: case 0xF6: Text += '"'; break;
            case 0xF7: Text += '~'; break;
            case 0xFA: case 0xFD: Text += "\xE2\x98\x85"; break; // star
            case 0xFE: Text += ' '; break; // new line
            default: break;
            }
        }
    }
    return std::string(); // no terminator, not text
}

static std::string CleanName(std::string Name, bool StripNumber)
{
    // Course names start with their number (" 1 BOB-OMB BATTLEFIELD")
    size_t Start = 0;
    while (Start < Name.size() && Name[Start] == ' ') { Start++; }
    if (StripNumber)
    {
        size_t Digits = Start;
        while (Digits < Name.size() && Name[Digits] >= '0' && Name[Digits] <= '9') { Digits++; }
        if (Digits > Start && (Digits == Name.size() || Name[Digits] == ' '))
        {
            Start = Digits;
            while (Start < Name.size() && Name[Start] == ' ') { Start++; }
        }
    }
    size_t End = Name.size();
    while (End > Start && Name[End - 1] == ' ') { End--; }
    Name = Name.substr(Start, End - Start);

    // The game prints everything in capitals; Title Case reads better in a list
    bool HasLower = false;
    for (size_t i = 0; i < Name.size(); i++) { HasLower = HasLower || (Name[i] >= 'a' && Name[i] <= 'z'); }
    if (!HasLower)
    {
        bool WordStart = true;
        for (size_t i = 0; i < Name.size(); i++)
        {
            char & c = Name[i];
            if (c >= 'A' && c <= 'Z' && !WordStart) { c = (char)(c - 'A' + 'a'); }
            WordStart = c == ' ' || c == '-' || c == '(' || c == '"';
        }
    }
    return Name;
}

bool CSm64Probe::ReadNames(const uint8_t * Rdram, uint32_t RdramSize, Names & Out)
{
    uint32_t Segment2 = ReadU32(Rdram, SM64_sSegmentTable + 2 * 4) & 0x1FFFFFFF;
    if (Segment2 == 0 || (Segment2 & 3) != 0 || Segment2 + SM64_ActNameTable + 15 * 6 * 4 > RdramSize)
    {
        return false;
    }

    auto ReadEntry = [&](uint32_t TableOffset, uint32_t Index, bool StripNumber, std::string & Name) -> bool
    {
        uint32_t Pointer = ReadU32(Rdram, Segment2 + TableOffset + Index * 4);
        if ((Pointer & 0xFFF00000) != 0x02000000)
        {
            return false;
        }
        uint32_t Addr = Segment2 + (Pointer & 0x000FFFFF);
        if (Addr >= RdramSize)
        {
            return false;
        }
        Name = CleanName(DecodeText(Rdram, RdramSize, Addr), StripNumber);
        return true;
    };

    int MainCourses = 0;
    for (uint32_t Course = 1; Course < CourseCount; Course++)
    {
        if (!ReadEntry(SM64_CourseNameTable, Course - 1, true, Out.Course[Course]))
        {
            return false;
        }
        if (Course <= 15 && !Out.Course[Course].empty())
        {
            MainCourses++;
        }
    }
    for (uint32_t Course = 1; Course <= 15; Course++)
    {
        for (uint32_t Act = 0; Act < 6; Act++)
        {
            if (!ReadEntry(SM64_ActNameTable, (Course - 1) * 6 + Act, false, Out.Act[Course][Act]))
            {
                return false;
            }
        }
    }
    // Before segment 2 is loaded the tables are garbage; real ones name most courses
    return MainCourses >= 8;
}

bool CSm64Probe::GetNames(Names & Out)
{
    CGuard Guard(m_CS);
    if (m_HaveNames)
    {
        Out = m_Names;
    }
    return m_HaveNames;
}

static uint64_t NowMs(void)
{
#ifdef _WIN32
    return GetTickCount64();
#else
    return 0;
#endif
}

static uint32_t CountReferences(const uint32_t * Words, uint32_t WordCount, uint16_t Hi, uint16_t Lo)
{
    // lui rX, Hi followed shortly by an instruction using the Lo offset
    uint32_t Count = 0;
    for (uint32_t i = 0; i + 4 < WordCount; i++)
    {
        if ((Words[i] >> 26) != 0x0F || (Words[i] & 0xFFFF) != Hi)
        {
            continue;
        }
        for (uint32_t j = i + 1; j <= i + 4; j++)
        {
            if ((Words[j] & 0xFFFF) == Lo)
            {
                Count++;
            }
        }
    }
    return Count;
}

void CSm64Probe::RomOpened(const uint8_t * Rom, uint32_t RomSize)
{
    bool Supported = false;
    if (Rom != nullptr && RomSize >= 0x200000)
    {
        // The engine code lives in the first 2MB, the ROM image is word swapped to native order
        const uint32_t * Words = (const uint32_t *)Rom;
        uint32_t WordCount = 0x200000 / sizeof(uint32_t);
        Supported = CountReferences(Words, WordCount, 0x8034, 0xBAC6) >= 20 &&
            CountReferences(Words, WordCount, 0x8034, 0xBAC8) >= 5 &&
            CountReferences(Words, WordCount, 0x8020, 0x7700) >= 10 &&
            CountReferences(Words, WordCount, 0x8033, 0xDDF4) >= 20;
    }

    CGuard Guard(m_CS);
    m_Supported = Supported;
    m_Resync = true;
    m_HaveStars = false;
    m_ViCount = 0;
    m_Snapshot.Valid = false;
    m_Grabs.clear();
    m_HaveNames = false;
}

void CSm64Probe::StateLoading(void)
{
    // A state can be from any point in time, so it must never look like stars were collected
    CGuard Guard(m_CS);
    m_Resync = true;
}

bool CSm64Probe::ReadStars(const uint8_t * Rdram, int16_t SaveFile, uint8_t (&Stars)[CourseCount])
{
    if (SaveFile < 1 || SaveFile > 4)
    {
        return false;
    }
    // Each save file is stored twice (main and backup)
    uint32_t File = SM64_gSaveBuffer + (SaveFile - 1) * 2 * SM64_SaveFileSize;
    Stars[0] = (uint8_t)((ReadU32(Rdram, File + SM64_SaveFileFlags) >> 24) & 0x7F);
    for (int Course = 1; Course < CourseCount; Course++)
    {
        Stars[Course] = ReadU8(Rdram, File + SM64_SaveFileCourseStars + (Course - 1)) & 0x7F;
    }
    return true;
}

void CSm64Probe::OnViRefresh(const uint8_t * Rdram, uint32_t RdramSize)
{
    if (!m_Supported || Rdram == nullptr || RdramSize < 0x400000)
    {
        return;
    }
    if (++m_ViCount < SampleEveryVi)
    {
        return;
    }
    m_ViCount = 0;

    int16_t SaveFile = ReadS16(Rdram, SM64_gCurrSaveFileNum);
    int16_t Course = ReadS16(Rdram, SM64_gCurrCourseNum);
    int16_t Act = ReadS16(Rdram, SM64_gCurrActNum);
    bool Valid = Course >= 0 && Course < CourseCount;
    uint8_t Stars[CourseCount];
    bool HaveStars = Valid && ReadStars(Rdram, SaveFile, Stars);

    // Names never change while a ROM runs, read them once segment 2 is in memory
    Names NewNames;
    bool GotNames = !m_HaveNames && ReadNames(Rdram, RdramSize, NewNames);

    CGuard Guard(m_CS);
    if (GotNames)
    {
        m_Names = NewNames;
        m_HaveNames = true;
    }
    m_Snapshot.Valid = Valid;
    m_Snapshot.Course = Course;
    m_Snapshot.Act = Act;
    m_Snapshot.TimeMs = NowMs();

    if (HaveStars && m_HaveStars && !m_Resync && SaveFile == m_SaveFile)
    {
        // A real star grab sets exactly one new bit, in the course being played.
        // Anything else (file copy, reset, menus) only moves the baseline.
        int NewBits = 0, GrabCourse = -1, GrabStar = -1;
        for (int c = 0; c < CourseCount; c++)
        {
            uint8_t Added = Stars[c] & ~m_Stars[c];
            for (int s = 0; s < StarsPerCourse; s++)
            {
                if (Added & (1 << s))
                {
                    NewBits++;
                    GrabCourse = c;
                    GrabStar = s;
                }
            }
        }
        if (NewBits == 1 && GrabCourse == Course)
        {
            StarGrab Grab = { (int16_t)GrabCourse, (int16_t)GrabStar, Act };
            m_Grabs.push_back(Grab);
        }
    }
    if (HaveStars)
    {
        memcpy(m_Stars, Stars, sizeof(m_Stars));
        m_SaveFile = SaveFile;
        m_Resync = false;
    }
    m_HaveStars = HaveStars;
}

bool CSm64Probe::Supported(void)
{
    CGuard Guard(m_CS);
    return m_Supported;
}

CSm64Probe::Snapshot CSm64Probe::GetSnapshot(void)
{
    CGuard Guard(m_CS);
    return m_Snapshot;
}

std::vector<CSm64Probe::StarGrab> CSm64Probe::TakeStarGrabs(void)
{
    CGuard Guard(m_CS);
    std::vector<StarGrab> Grabs;
    Grabs.swap(m_Grabs);
    return Grabs;
}
