#pragma once
#include <Common/path.h>
#include <Common/CriticalSection.h>
#include <map>
#include <string>

// Extra per-slot information for instant save states: a screenshot thumbnail
// taken when the state was saved, and a user editable label.
//
// Thumbnails are stored in a "Thumbnails" sub directory of the save state
// directory, so they never show up in the "Load State" file filter (*.pj*).
// Labels are stored in "<GoodName>.labels" next to the save states, so they
// follow the states if the directory is moved or shared.

class CSaveStateInfo
{
public:
    enum
    {
        ThumbnailWidth = 320,
    };

    static CPath StateDirectory(void);
    static CPath StateFile(uint32_t Slot);
    static CPath ThumbnailFile(uint32_t Slot);

    static std::string GetLabel(uint32_t Slot);
    static void SetLabel(uint32_t Slot, const char * Label);

    // Called right after a state was saved. Asks the main window (CaptureMessage)
    // to take the screenshot, which then calls WriteThumbnail on the UI thread.
    static void CaptureThumbnail(uint32_t Slot);
    static void WriteThumbnail(uint32_t Slot);
    static uint32_t CaptureMessage(void);

private:
    static CPath LabelsFile(void);
    static void LoadLabels(const CPath & File);

    static CriticalSection m_CS;
    static std::string m_LabelsFileName;
    static uint64_t m_LabelsFileTime;
    static std::map<uint32_t, std::string> m_Labels;
};
