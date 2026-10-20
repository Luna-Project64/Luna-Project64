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
    if (!file)
        return;

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
    zipWriteInFileInZip(file, SerialInterface, sizeof(uint32_t) * 4);
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

namespace
{

bool ReadExact(unzFile File, void * Buffer, uint32_t Length)
{
    return unzReadCurrentFile(File, Buffer, Length) == (int)Length;
}

bool ReadMainState(unzFile File, MemoryState & State)
{
    if (!ReadExact(File, &State.RdramSize, sizeof(uint32_t)) ||
        State.RdramSize == 0 || State.RdramSize > sizeof(State.Rdram))
    {
        return false;
    }
    return ReadExact(File, State.RomData, 0x40) &&
        ReadExact(File, &State.NextViTimer, sizeof(uint32_t)) &&
        ReadExact(File, &State.ProgramCounter, sizeof(State.ProgramCounter)) &&
        ReadExact(File, State.GPR, sizeof(int64_t) * 32) &&
        ReadExact(File, State.FPR, sizeof(int64_t) * 32) &&
        ReadExact(File, State.CP0, sizeof(uint32_t) * 32) &&
        ReadExact(File, State.FPCR, sizeof(uint32_t) * 32) &&
        ReadExact(File, &State.HI, sizeof(int64_t)) &&
        ReadExact(File, &State.LO, sizeof(int64_t)) &&
        ReadExact(File, State.RDRAM_Registers, sizeof(uint32_t) * 10) &&
        ReadExact(File, State.SigProcessor_Interface, sizeof(uint32_t) * 10) &&
        ReadExact(File, State.Display_ControlReg, sizeof(uint32_t) * 10) &&
        ReadExact(File, State.Mips_Interface, sizeof(uint32_t) * 4) &&
        ReadExact(File, State.Video_Interface, sizeof(uint32_t) * 14) &&
        ReadExact(File, State.Audio_Interface, sizeof(uint32_t) * 6) &&
        ReadExact(File, State.Peripheral_Interface, sizeof(uint32_t) * 13) &&
        ReadExact(File, State.RDRAM_Interface, sizeof(uint32_t) * 8) &&
        ReadExact(File, State.SerialInterface, sizeof(uint32_t) * 4) &&
        ReadExact(File, State.TLB, sizeof(CTLB::TLB_ENTRY) * 32) &&
        ReadExact(File, State.PifRam, 0x40) &&
        ReadExact(File, State.Rdram, State.RdramSize) &&
        ReadExact(File, State.Dmem, 0x1000) &&
        ReadExact(File, State.Imem, 0x1000);
}

bool ReadExtraState(unzFile File, MemoryState & State)
{
    uint32_t TimerDetailsSize = 0, Entries = 0;
    if (!ReadExact(File, State.DiskInterface, sizeof(uint32_t) * 22) ||
        !ReadExact(File, &TimerDetailsSize, sizeof(TimerDetailsSize)) ||
        !ReadExact(File, &Entries, sizeof(Entries)))
    {
        return false;
    }
    MemoryState::TimerState & Timers = State.TimerStat;
    if (TimerDetailsSize != sizeof(Timers.Detatils[0]) || Entries != sizeof(Timers.Detatils) / sizeof(Timers.Detatils[0]))
    {
        return false;
    }
    return ReadExact(File, Timers.Detatils, sizeof(Timers.Detatils)) &&
        ReadExact(File, &Timers.LastUpdate, sizeof(Timers.LastUpdate)) &&
        ReadExact(File, &Timers.Next, sizeof(Timers.Next)) &&
        ReadExact(File, &Timers.Current, sizeof(Timers.Current));
}

} // namespace

std::shared_ptr<MemoryState> MemoryState::Load(const char * ZipFile, uint32_t MainId, uint32_t ExtraId)
{
    unzFile File = unzOpen(ZipFile);
    if (File == nullptr)
    {
        return nullptr;
    }

    // Every field is overwritten below, so skip zeroing 8MB up front
    std::shared_ptr<MemoryState> State(new MemoryState);
    bool HaveMain = false, HaveExtra = false, Failed = false;
    for (int Result = unzGoToFirstFile(File); Result == UNZ_OK && !Failed; Result = unzGoToNextFile(File))
    {
        if (unzOpenCurrentFile(File) != UNZ_OK)
        {
            Failed = true;
            break;
        }
        uint32_t Id = 0;
        if (ReadExact(File, &Id, sizeof(Id)))
        {
            if (Id == MainId && !HaveMain)
            {
                State->SaveID_0 = Id;
                HaveMain = ReadMainState(File, *State);
                Failed = !HaveMain;
            }
            else if (Id == ExtraId && !HaveExtra)
            {
                State->SaveID_2 = Id;
                HaveExtra = ReadExtraState(File, *State);
                Failed = !HaveExtra;
            }
        }
        if (unzCloseCurrentFile(File) != UNZ_OK)
        {
            // A CRC mismatch is reported here, after the data has been read
            Failed = true;
        }
    }
    unzClose(File);

    if (Failed || !HaveMain || !HaveExtra)
    {
        return nullptr;
    }
    return State;
}
