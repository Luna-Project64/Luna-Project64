#include "stdafx.h"
#include <Common/MemoryManagement.h>

#include <Project64-core/N64System/Mips/Dma.h>
#include <Project64-core/N64System/SystemGlobals.h>
#include <Project64-core/N64System/N64Rom.h>
#include <Project64-core/N64System/Mips/MemoryVirtualMem.h>
#include <Project64-core/N64System/Mips/Register.h>
#include <Project64-core/N64System/Mips/Disk.h>
#include <Project64-core/N64System/N64Disk.h>
#include <Project64-core/N64System/N64System.h>
#include <Project64-core/N64System/SummerCart.h>

CDMA::CDMA(CFlashram & FlashRam, CSram & Sram) :
    m_FlashRam(FlashRam),
    m_Sram(Sram)
{
}

void CDMA::OnFirstDMA()
{
    int16_t offset;
    const uint32_t base = 0x00000000;
    const uint32_t rt = g_MMU->RdramSize();

    switch (g_Rom->CicChipID())
    {
    case CIC_NUS_6101:  offset = +0x0318; break;
    case CIC_NUS_5167:  offset = +0x0318; break;
    case CIC_NUS_8303:  offset = +0x0318; break;
    case CIC_NUS_DDUS:  offset = +0x0318; break;
    case CIC_NUS_DDTL:  offset = +0x0318; break;
    case CIC_UNKNOWN:
    case CIC_NUS_6102:  offset = +0x0318; break;
    case CIC_NUS_6103:  offset = +0x0318; break;
    case CIC_NUS_6105:  offset = +0x03F0; break;
    case CIC_NUS_6106:  offset = +0x0318; break;
    default:
        g_Notify->DisplayError(stdstr_f("Unhandled CicChip(%d) in first DMA", g_Rom->CicChipID()).c_str());
        return;
    }
    g_MMU->SW_PAddr(base + offset, rt);
}

void CDMA::PI_DMA_READ()
{
    //	PI_STATUS_REG |= PI_STATUS_DMA_BUSY;
    uint32_t PI_RD_LEN_REG = ((g_Reg->PI_RD_LEN_REG) & 0x00FFFFFFul) + 1;
    uint32_t PI_DRAM_ADDR_REG = g_Reg->PI_DRAM_ADDR_REG & 0x00FFFFFF;

    if ((PI_RD_LEN_REG & 1) != 0)
    {
        PI_RD_LEN_REG += 1;
    }

    if (PI_DRAM_ADDR_REG + PI_RD_LEN_REG > g_MMU->RdramSize())
    {
        if (HaveDebugger())
        {
            g_Notify->DisplayError(stdstr_f("PI_DMA_READ not in Memory: %08X", PI_DRAM_ADDR_REG + PI_RD_LEN_REG).c_str());
        }
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    // 64DD buffers write
    if (g_Reg->PI_CART_ADDR_REG >= 0x05000000 && g_Reg->PI_CART_ADDR_REG <= 0x050003FF)
    {
        // 64DD C2 sectors (don't care)
        g_SystemTimer->SetTimer(g_SystemTimer->DDPiTimer, (PI_RD_LEN_REG * 63) / 25, false);
        return;
    }

    if (g_Reg->PI_CART_ADDR_REG >= 0x05000400 && g_Reg->PI_CART_ADDR_REG <= 0x050004FF)
    {
        // 64DD user sector
        uint32_t i;
        uint8_t * RDRAM = g_MMU->Rdram();
        uint8_t * DISK = g_Disk->GetDiskAddressBuffer();
        for (i = 0; i < PI_RD_LEN_REG; i++)
        {
            *(DISK + (i ^ 3)) = *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3));
        }
        g_SystemTimer->SetTimer(g_SystemTimer->DDPiTimer, (PI_RD_LEN_REG * 63) / 25, false);
        return;
    }

    if (g_Reg->PI_CART_ADDR_REG >= 0x05000580 && g_Reg->PI_CART_ADDR_REG <= 0x050005BF)
    {
        // 64DD MSEQ (don't care)
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    // Write ROM area (for 64DD conversion)
    if (g_Reg->PI_CART_ADDR_REG >= 0x10000000 && g_Reg->PI_CART_ADDR_REG <= 0x1FBFFFFF && g_System->bAllowROMWrites())
    {
        uint32_t i;
        uint8_t * ROM = g_Rom->GetRomAddress();
        uint8_t * RDRAM = g_MMU->Rdram();

        ProtectMemory(ROM, g_Rom->GetRomSize(), MEM_READWRITE);
        g_Reg->PI_CART_ADDR_REG -= 0x10000000;
        if (g_Reg->PI_CART_ADDR_REG + PI_RD_LEN_REG < g_Rom->GetRomSize())
        {
            for (i = 0; i < PI_RD_LEN_REG; i++)
            {
                *(ROM + ((g_Reg->PI_CART_ADDR_REG + i) ^ 3)) = *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3));
            }
        }
        else
        {
            uint32_t Len;
            Len = g_Rom->GetRomSize() - g_Reg->PI_CART_ADDR_REG;
            for (i = 0; i < Len; i++)
            {
                *(ROM + ((g_Reg->PI_CART_ADDR_REG + i) ^ 3)) = *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3));
            }
        }
        g_Reg->PI_CART_ADDR_REG += 0x10000000;

        if (!g_System->DmaUsed())
        {
            g_System->SetDmaUsed(true);
            OnFirstDMA();
        }
        if (g_Recompiler && g_System->bSMM_PIDMA())
        {
            g_Recompiler->ClearRecompCode_Phys(PI_DRAM_ADDR_REG, g_Reg->PI_WR_LEN_REG, CRecompiler::Remove_DMA);
        }

        ProtectMemory(ROM, g_Rom->GetRomSize(), MEM_READONLY);

        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    if (g_Reg->PI_CART_ADDR_REG >= 0x1ffe0000 && g_Reg->PI_CART_ADDR_REG < 0x1fff0000)
    {
        DWORD length = (PI_RD_LEN_REG & 0xFFFFFF) + 1;
        DWORD i = (g_Reg->PI_CART_ADDR_REG - 0x1ffe0000);

        length = (i + length) > 8192 ? (8192 - i) : length;
        length = (PI_DRAM_ADDR_REG + length) > 0x7FFFFF ?
            (0x7FFFFF - PI_DRAM_ADDR_REG) : length;

        if (i > 8192 || PI_DRAM_ADDR_REG > 0x7FFFFF || !g_SummerCart->Unlocked())
        {
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
            return;
        }

        DWORD dram_address = PI_DRAM_ADDR_REG;
        DWORD rom_address = (g_Reg->PI_CART_ADDR_REG - 0x1ffe0000);
        BYTE* dram = g_MMU->Rdram();
        BYTE* rom = g_SummerCart->Buffer();

        for (i = 0; i < length; ++i)
            rom[(rom_address + i) ^ 3] = dram[(dram_address + i) ^ 3];

        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    if (g_Reg->PI_CART_ADDR_REG >= 0x08000000 && g_Reg->PI_CART_ADDR_REG <= 0x08010000)
    {
        if (g_System->m_SaveUsing == SaveChip_Auto)
        {
            g_System->m_SaveUsing = SaveChip_Sram;
        }
        if (g_System->m_SaveUsing == SaveChip_Sram)
        {
            m_Sram.DmaToSram(
                g_MMU->Rdram() + PI_DRAM_ADDR_REG,
                g_Reg->PI_CART_ADDR_REG - 0x08000000,
                PI_RD_LEN_REG
                );
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
            return;
        }
        if (g_System->m_SaveUsing == SaveChip_FlashRam)
        {
            m_FlashRam.DmaToFlashram(
                g_MMU->Rdram() + PI_DRAM_ADDR_REG,
                g_Reg->PI_CART_ADDR_REG - 0x08000000,
                PI_RD_LEN_REG
                );
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
            return;
        }
    }
    if (g_System->m_SaveUsing == SaveChip_FlashRam)
    {
        g_Notify->DisplayError(stdstr_f("**** FlashRAM DMA read address %08X ****", g_Reg->PI_CART_ADDR_REG).c_str());
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }
    if (HaveDebugger())
    {
        g_Notify->DisplayError(stdstr_f("PI_DMA_READ where are you DMAing to? : %08X", g_Reg->PI_CART_ADDR_REG).c_str());
    }
    g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
    g_Reg->MI_INTR_REG |= MI_INTR_PI;
    g_Reg->CheckInterrupts();
    return;
}

void CDMA::PI_DMA_WRITE()
{
    // Rounding PI_WR_LEN_REG up to the nearest even number fixes AI Shougi 3, Doraemon 3, etc.
    uint32_t PI_WR_LEN_REG = ((g_Reg->PI_WR_LEN_REG) & 0x00FFFFFEul) + 2;
    uint32_t PI_CART_ADDR_REG = !g_System->bUnalignedDMA() ? g_Reg->PI_CART_ADDR_REG & ~1 : g_Reg->PI_CART_ADDR_REG;
    uint32_t PI_DRAM_ADDR_REG = g_Reg->PI_DRAM_ADDR_REG & 0x00FFFFFF;

    g_Reg->PI_STATUS_REG |= PI_STATUS_DMA_BUSY;
    if (PI_DRAM_ADDR_REG + PI_WR_LEN_REG > g_MMU->RdramSize())
    {
        if (ShowUnhandledMemory()) { g_Notify->DisplayError(stdstr_f("PI_DMA_WRITE not in memory: %08X", PI_DRAM_ADDR_REG + PI_WR_LEN_REG).c_str()); }
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    // 64DD buffers read
    if (PI_CART_ADDR_REG >= 0x05000000 && PI_CART_ADDR_REG <= 0x050003FF)
    {
        // 64DD C2 sectors (just read 0)
        uint32_t i;
        uint8_t * RDRAM = g_MMU->Rdram();
        for (i = 0; i < PI_WR_LEN_REG; i++)
        {
            *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = 0;
        }

        // Timer is needed for track read
        g_SystemTimer->SetTimer(g_SystemTimer->DDPiTimer, (PI_WR_LEN_REG * 63) / 25, false);
        return;
    }

    if (PI_CART_ADDR_REG >= 0x05000400 && PI_CART_ADDR_REG <= 0x050004FF)
    {
        // 64DD user sector
        uint32_t i;
        uint8_t * RDRAM = g_MMU->Rdram();
        uint8_t * DISK = g_Disk->GetDiskAddressBuffer();
        for (i = 0; i < PI_WR_LEN_REG; i++)
        {
            *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = *(DISK + (i ^ 3));
        }

        // Timer is needed for track read
        g_SystemTimer->SetTimer(g_SystemTimer->DDPiTimer, (PI_WR_LEN_REG * 63) / 25, false);
        return;
    }

    if (PI_CART_ADDR_REG >= 0x05000580 && PI_CART_ADDR_REG <= 0x050005BF)
    {
        // 64DD MSEQ (don't care)
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        return;
    }

    // 64DD IPL ROM
    if (PI_CART_ADDR_REG >= 0x06000000 && PI_CART_ADDR_REG <= 0x063FFFFF)
    {
        uint32_t i;

        uint8_t * ROM = g_DDRom->GetRomAddress();
        uint8_t * RDRAM = g_MMU->Rdram();
        PI_CART_ADDR_REG -= 0x06000000;
        if (PI_CART_ADDR_REG + PI_WR_LEN_REG < g_DDRom->GetRomSize())
        {
            for (i = 0; i < PI_WR_LEN_REG; i++)
            {
                *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = *(ROM + ((PI_CART_ADDR_REG + i) ^ 3));
            }
        }
        else if (PI_CART_ADDR_REG >= g_DDRom->GetRomSize())
        {
            uint32_t cart = PI_CART_ADDR_REG - g_DDRom->GetRomSize();
            while (cart >= g_DDRom->GetRomSize())
            {
                cart -= g_DDRom->GetRomSize();
            }
            for (i = 0; i < PI_WR_LEN_REG; i++)
            {
                *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = *(ROM + ((cart + i) ^ 3));
            }
        }
        else
        {
            uint32_t Len;
            Len = g_DDRom->GetRomSize() - PI_CART_ADDR_REG;
            for (i = 0; i < Len; i++)
            {
                *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = *(ROM + ((PI_CART_ADDR_REG + i) ^ 3));
            }
            for (i = Len; i < PI_WR_LEN_REG - Len; i++)
            {
                *(RDRAM + ((PI_DRAM_ADDR_REG + i) ^ 3)) = 0;
            }
        }
        PI_CART_ADDR_REG += 0x06000000;

        if (!g_System->DmaUsed())
        {
            g_System->SetDmaUsed(true);
            OnFirstDMA();
        }
        if (g_Recompiler && g_System->bSMM_PIDMA())
        {
            g_Recompiler->ClearRecompCode_Phys(PI_DRAM_ADDR_REG, g_Reg->PI_WR_LEN_REG, CRecompiler::Remove_DMA);
        }
        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        //ChangeTimer(PiTimer,(int32_t)(PI_WR_LEN_REG * 8.9) + 50);
        //ChangeTimer(PiTimer,(int32_t)(PI_WR_LEN_REG * 8.9));
        return;
    }

    if (PI_CART_ADDR_REG >= 0x08000000 && PI_CART_ADDR_REG <= 0x08088000)
    {
        if (g_System->m_SaveUsing == SaveChip_Auto)
        {
            g_System->m_SaveUsing = SaveChip_Sram;
        }
        if (g_System->m_SaveUsing == SaveChip_Sram)
        {
            m_Sram.DmaFromSram(
                g_MMU->Rdram() + PI_DRAM_ADDR_REG,
                PI_CART_ADDR_REG - 0x08000000,
                PI_WR_LEN_REG
                );
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
            return;
        }
        if (g_System->m_SaveUsing == SaveChip_FlashRam)
        {
            m_FlashRam.DmaFromFlashram(
                g_MMU->Rdram() + PI_DRAM_ADDR_REG,
                PI_CART_ADDR_REG - 0x08000000,
                PI_WR_LEN_REG
                );
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
        }
        return;
    }

    if (PI_CART_ADDR_REG >= 0x10000000 && PI_CART_ADDR_REG < 0x1FFE0000)
    {
        uint8_t* Rdram = g_MMU->Rdram();
        uint32_t RdramSize = g_MMU->RdramSize();
        uint8_t* ROM = g_Rom->GetRomAddress();
        uint32_t RomSize = g_Rom->GetRomSize();

        int32_t Length = (g_Reg->PI_WR_LEN_REG & 0x00FFFFFF) + 1;
        g_Reg->PI_WR_LEN_REG = Length <= 8 ? 0x7F - (PI_DRAM_ADDR_REG & 7) : 0x7F;

        uint32_t ReadPos = PI_CART_ADDR_REG - 0x10000000;
        bool canUseMemcpy = (PI_DRAM_ADDR_REG & 7) == 0 &&
                            (ReadPos & 3) == 0 &&
                            ReadPos + Length <= RomSize &&
                            PI_DRAM_ADDR_REG + Length <= RdramSize;

        uint32_t invalidationStart = PI_DRAM_ADDR_REG;

        uint32_t TransferLen = 0;
        if (canUseMemcpy)
        {
            if (Length > 0 && (Length & 7) == 0)
            {
                memcpy(Rdram + PI_DRAM_ADDR_REG, ROM + ReadPos, Length);
                PI_CART_ADDR_REG += Length;
                PI_DRAM_ADDR_REG += Length;
                TransferLen = Length;
                Length = 0;
            }
#if 0
            else
            {
                int32_t bulkLen = Length & ~127u;
                if (bulkLen)
                {
                    memcpy(Rdram + PI_DRAM_ADDR_REG, ROM + ReadPos, bulkLen);
                    PI_CART_ADDR_REG += bulkLen;
                    PI_DRAM_ADDR_REG += bulkLen;
                    Length -= bulkLen;
                    TransferLen += bulkLen;
                }
            }
#endif
        }

        if (Length > 0)
        {
            uint8_t Block[128];
            bool FirstBlock = TransferLen == 0;
            int32_t MaxBlockSize = 128;
            while (Length > 0)
            {
                int32_t BlockAlign = PI_DRAM_ADDR_REG & 7;
                int32_t BlockSize = MaxBlockSize - BlockAlign;
                int32_t BlockLen = BlockSize;
                if (Length < BlockLen)
                {
                    BlockLen = Length;
                }
                int32_t EndOfRow = 0x800 - (PI_DRAM_ADDR_REG & 0x7ff);
                if (EndOfRow < BlockLen)
                {
                    BlockLen = EndOfRow;
                }
                Length -= BlockLen;
                if (Length < 0)
                {
                    Length = 0;
                }

                int32_t ReadLen = (BlockLen + 1) & ~1;

                {
                    uint32_t ReadPos = PI_CART_ADDR_REG - 0x10000000;
                    for (uint32_t i = 0, n = (BlockLen + 1) & ~1; i < n; i++)
                    {
                        uint32_t Pos = ((ReadPos + i) ^ 3);
                        Block[i] = Pos < RomSize ? ROM[Pos] : 0;
                    }
                }

                PI_CART_ADDR_REG += ReadLen;

                if (FirstBlock)
                {
                    if (BlockLen == BlockSize - 1)
                    {
                        BlockLen += 1;
                    }
                    BlockLen = BlockLen - BlockAlign;
                    if (BlockLen < 0)
                    {
                        BlockLen = 0;
                    }
                }
                else
                {
                    BlockLen = ReadLen;
                }

                if ((PI_DRAM_ADDR_REG + BlockLen) >= RdramSize)
                {
                    BlockLen = RdramSize - PI_DRAM_ADDR_REG;
                    if (BlockLen < 0)
                    {
                        BlockLen = 0;
                    }
                }
                for (int32_t i = 0; i < BlockLen; i++)
                {
                    Rdram[(PI_DRAM_ADDR_REG + i) ^ 3] = Block[i];
                }
                PI_DRAM_ADDR_REG = (PI_DRAM_ADDR_REG + BlockLen + 7) & ~7;
                TransferLen += (BlockLen + 7) & ~7;
                MaxBlockSize = EndOfRow < 8 ? 128 - BlockAlign : 128;
                FirstBlock = false;
            }
        }
        g_Reg->PI_CART_ADDR_REG = PI_CART_ADDR_REG;
        g_Reg->PI_DRAM_ADDR_REG = PI_DRAM_ADDR_REG;

        if (!g_System->DmaUsed())
        {
            g_System->SetDmaUsed(true);
            OnFirstDMA();
        }
        if (g_Recompiler && g_System->bSMM_PIDMA())
        {
            g_Recompiler->ClearRecompCode_Phys(invalidationStart, TransferLen, CRecompiler::Remove_DMA);
        }

        if(g_System->bRandomizeSIPIInterrupts())
        {
            g_SystemTimer->SetTimer(g_SystemTimer->PiTimer, TransferLen / 8 + (g_Random->next() % 0x40), false);
        }
        else
        {
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
        }
        //ChangeTimer(PiTimer,(int32_t)(PI_WR_LEN_REG * 8.9) + 50);
        //ChangeTimer(PiTimer,(int32_t)(PI_WR_LEN_REG * 8.9));
        return;
    }

    if (PI_CART_ADDR_REG >= 0x1FFE0000 && PI_CART_ADDR_REG < 0x1FFF0000)
    {
        /* SC64 BUFFER */
        uint32_t length = (PI_WR_LEN_REG & 0xFFFFFE) + 2;
        uint32_t i = (PI_CART_ADDR_REG - 0x1ffe0000);
        length = (i + length) > 8192 ? (8192 - i) : length;
        length = (PI_DRAM_ADDR_REG + length) > 0x7FFFFF ?
            (0x7FFFFF - PI_DRAM_ADDR_REG) : length;

        if (i > 8192 || PI_DRAM_ADDR_REG > 0x7FFFFF || !g_SummerCart->Unlocked())
        {
            g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
            g_Reg->MI_INTR_REG |= MI_INTR_PI;
            g_Reg->CheckInterrupts();
            return;
        }

        uint32_t dram_address = PI_DRAM_ADDR_REG;
        uint32_t rom_address = (PI_CART_ADDR_REG - 0x1ffe0000);
        uint8_t* dram = g_MMU->Rdram();
        uint8_t* rom = g_SummerCart->Buffer();

        for (i = 0; i < length; ++i)
            dram[(dram_address + i) ^ 3] = rom[(rom_address + i) ^ 3];

        g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
        g_Reg->MI_INTR_REG |= MI_INTR_PI;
        g_Reg->CheckInterrupts();
        // This function is gone from the 3.x version and I have no clue what did they mean by this...
        // g_SystemTimer->UpdateTimers();
        return;
    }

    if (ShowUnhandledMemory())
    {
        g_Notify->DisplayError(stdstr_f("PI_DMA_WRITE not in ROM: %08X", PI_CART_ADDR_REG).c_str());
    }
    g_Reg->PI_STATUS_REG &= ~PI_STATUS_DMA_BUSY;
    g_Reg->MI_INTR_REG |= MI_INTR_PI;
    g_Reg->CheckInterrupts();
}

void CDMA::SP_DMA_READ()
{
    uint32_t length = ((g_Reg->SP_RD_LEN_REG & 0xFFF) | 7) + 1;
    uint32_t skip   = (g_Reg->SP_RD_LEN_REG >> 20) & 0xFF8;
    unsigned count  = ((g_Reg->SP_RD_LEN_REG >> 12) & 0xFF) + 1;

    unsigned i = 0;
    uint32_t rdram = g_Reg->SP_DRAM_ADDR_REG & 0xfffff8;
    uint32_t spmem = g_Reg->SP_MEM_ADDR_REG & 0xff8;
    bool imem = g_Reg->SP_MEM_ADDR_REG & 0x1000;
    uint8_t* rsp_ptr = imem ? g_MMU->Imem() : g_MMU->Dmem();

    if ((0 == skip || 1 == count) && (spmem + length) < 0x1000 && (rdram + length) < g_MMU->RdramSize())
    {
        // TODO: Sane case should probably also include the case where skip == 0 but i do not care enough
        uint8_t* rdram_ptr = g_MMU->Rdram() + rdram;
        memcpy(rsp_ptr + spmem, rdram_ptr, length);
        rdram += length;
        spmem += length;
    }
    else
    {
        do
        {
            unsigned j = 0;
            do
            {
                uint32_t source_addr = rdram + j;
                uint32_t dest_addr = (spmem + j) & 0xfff;
                uint64_t word = source_addr >= g_MMU->RdramSize() ? 0 : *(uint64_t*)(g_MMU->Rdram() + source_addr);
                *(uint64_t*)(rsp_ptr + dest_addr) = word;

                j += 8;
            } while (j < length);

            rdram += length + skip;
            spmem += length;
        } while (++i < count);
    }

    g_Reg->SP_DRAM_ADDR_REG = rdram;
    g_Reg->SP_MEM_ADDR_REG = spmem & 0xff8;
    g_Reg->SP_RD_LEN_REG = 0xff8;

    g_Reg->SP_DMA_BUSY_REG = 0;
    g_Reg->SP_STATUS_REG &= ~SP_STATUS_DMA_BUSY;
}

void CDMA::SP_DMA_WRITE()
{
    uint32_t length = ((g_Reg->SP_WR_LEN_REG & 0xFFF) | 7) + 1;
    uint32_t skip = (g_Reg->SP_WR_LEN_REG >> 20) & 0xFF8;
    unsigned count = ((g_Reg->SP_WR_LEN_REG >> 12) & 0xFF) + 1;

    unsigned i = 0;
    uint32_t rdram = g_Reg->SP_DRAM_ADDR_REG & 0xfffff8;
    uint32_t spmem = g_Reg->SP_MEM_ADDR_REG & 0xff8;
    bool imem = g_Reg->SP_MEM_ADDR_REG & 0x1000;
    uint8_t* rsp_ptr = imem ? g_MMU->Imem() : g_MMU->Dmem();

    if ((0 == skip || 1 == count) && (spmem + length) <= 0x1000 && (rdram + length) <= g_MMU->RdramSize())
    {
        // TODO: Sane case should probably also include the case where skip == 0 but i do not care enough
        uint8_t* rdram_ptr = g_MMU->Rdram() + rdram;
        memcpy(rdram_ptr, rsp_ptr + spmem, length);
        rdram += length;
        spmem += length;
    }
    else
    {
        do
        {
            unsigned j = 0;
            do
            {
                uint32_t dest_addr = rdram + j;
                uint32_t source_addr = (spmem + j) & 0xfff;
                uint64_t word = *(uint64_t*)(rsp_ptr + source_addr);
                if (dest_addr < g_MMU->RdramSize())
                    *(uint64_t*)(g_MMU->Rdram() + dest_addr) = word;

                j += 8;
            } while (j < length);

            rdram += length + skip;
            spmem += length;
        } while (++i < count);
    }

    g_Reg->SP_DRAM_ADDR_REG = rdram;
    g_Reg->SP_MEM_ADDR_REG = spmem & 0xff8;
    g_Reg->SP_WR_LEN_REG = 0xff8;

    g_Reg->SP_DMA_BUSY_REG = 0;
    g_Reg->SP_STATUS_REG &= ~SP_STATUS_DMA_BUSY;
}