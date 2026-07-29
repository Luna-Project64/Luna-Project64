#include "MemoryState.h"

#include <Common/StdString.h>
#include <Project64-core/3rdParty/zip.h>
#include <Project64-core/N64System/Mips/SystemTiming.h>

void MemoryState::store(CPath& SaveFile, const CPath& ExtraInfo)
{
    if (!SaveFile.DirectoryExists())
    {
        SaveFile.DirectoryCreate();
    }

    CPath ZipFile(SaveFile);
    ZipFile.SetNameExtension(stdstr_f("%s.zip", ZipFile.GetNameExtension().c_str()).c_str());

    ZipFile.Delete();
    zipFile file = zipOpen(ZipFile, 0);
    zipOpenNewFileInZip(file, SaveFile.GetNameExtension().c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
    zipWriteInFileInZip(file, &SaveID_0, sizeof(SaveID_0));
    zipWriteInFileInZip(file, &RdramSize, sizeof(uint32_t));
    zipWriteInFileInZip(file, RomData, 0x40);
    zipWriteInFileInZip(file, &NextViTimer, sizeof(uint32_t));
    zipWriteInFileInZip(file, &ProgramCounter, sizeof(ProgramCounter));
    zipWriteInFileInZip(file, GPR, sizeof(int64_t) * 32);
    zipWriteInFileInZip(file, FPR, sizeof(int64_t) * 32);
    zipWriteInFileInZip(file, CP0, sizeof(uint32_t) * 32);
    zipWriteInFileInZip(file, FPCR, sizeof(uint32_t) * 32);
    zipWriteInFileInZip(file, &HI, sizeof(int64_t));
    zipWriteInFileInZip(file, &LO, sizeof(int64_t));
    zipWriteInFileInZip(file, RDRAM_Registers, sizeof(uint32_t) * 10);
    zipWriteInFileInZip(file, SigProcessor_Interface, sizeof(uint32_t) * 10);
    zipWriteInFileInZip(file, Display_ControlReg, sizeof(uint32_t) * 10);
    zipWriteInFileInZip(file, Mips_Interface, sizeof(uint32_t) * 4);
    zipWriteInFileInZip(file, Video_Interface, sizeof(uint32_t) * 14);
    zipWriteInFileInZip(file, Audio_Interface, sizeof(uint32_t) * 6);
    zipWriteInFileInZip(file, Peripheral_Interface, sizeof(uint32_t) * 13);
    zipWriteInFileInZip(file, RDRAM_Interface, sizeof(uint32_t) * 8);
    zipWriteInFileInZip(file, SigProcessor_Interface, sizeof(uint32_t) * 4);
    zipWriteInFileInZip(file, TLB, sizeof(CTLB::TLB_ENTRY) * 32);
    zipWriteInFileInZip(file, PifRam, 0x40);
    zipWriteInFileInZip(file, Rdram, RdramSize);
    zipWriteInFileInZip(file, Dmem, 0x1000);
    zipWriteInFileInZip(file, Imem, 0x1000);
    zipCloseFileInZip(file);

    zipOpenNewFileInZip(file, ExtraInfo.GetNameExtension().c_str(), nullptr, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION);

    // Extra info v2
    zipWriteInFileInZip(file, &SaveID_2, sizeof(SaveID_2));

    // Disk interface info
    zipWriteInFileInZip(file, DiskInterface, sizeof(uint32_t) * 22);

    // System timers info
    CSystemTimer::SaveData(file, TimerStat);

    zipCloseFileInZip(file);

    zipClose(file, "");
}
