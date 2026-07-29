#pragma once

#include <stdint.h>

#ifndef CALL
#ifdef _WIN32
#define CALL __cdecl
#else
#define CALL
#endif
#endif

#include <Project64-core/N64System/Mips/TLB.h>
#include <Common/path.h>

struct MemoryState
{
public:
    MemoryState() = default;

    void store(CPath& SaveFile, const CPath& ExtraInfo);

    struct TimerState
    {
        struct TimerDetails
        {
            int64_t stub[2];
        };

        TimerDetails Detatils[14];
        int32_t LastUpdate;
        int32_t Next;
        int Current;
    };

    uint32_t SaveID_0;
    uint32_t RdramSize;

    uint8_t RomData[0x40];
    uint32_t NextViTimer;
    uint32_t ProgramCounter;
    int64_t GPR[32];
    int64_t FPR[32];
    uint32_t CP0[32];
    uint32_t FPCR[32];
    int64_t HI;
    int64_t LO;
    uint32_t RDRAM_Registers[10];
    uint32_t SigProcessor_Interface[10];
    uint32_t Display_ControlReg[10];
    uint32_t Mips_Interface[4];
    uint32_t Video_Interface[14];
    uint32_t Audio_Interface[6];
    uint32_t Peripheral_Interface[13];
    uint32_t RDRAM_Interface[8];
    uint32_t SerialInterface[4];
    CTLB::TLB_ENTRY TLB[32];
    uint8_t PifRam[0x40];
    uint8_t Rdram[0x800000];
    uint8_t Dmem[0x1000];
    uint8_t Imem[0x1000];

    uint32_t SaveID_2;
    uint32_t DiskInterface[22];

    TimerState TimerStat;
};
