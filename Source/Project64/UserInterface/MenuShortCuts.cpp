#include "stdafx.h"

VIRTUAL_KEY CMenuShortCutKey::m_VirtualKeyList[] = {
    { L"VK_LBUTTON", 0x01, L"VK_LBUTTON" },
    { L"VK_RBUTTON", 0x02, L"VK_RBUTTON" },
    { L"VK_CANCEL", 0x03, L"VK_CANCEL" },
    { L"VK_MBUTTON", 0x04, L"VK_MBUTTON" },
    { L"VK_XBUTTON1", 0x05, L"VK_XBUTTON1" },
    { L"VK_XBUTTON2", 0x06, L"VK_XBUTTON2" },
    { L"VK_BACK", 0x08, L"VK_BACK" },
    { L"VK_TAB", 0x09, L"VK_TAB" },
    { L"VK_CLEAR", 0x0C, L"VK_CLEAR" },
    { L"VK_RETURN", 0x0D, L"Return" },
    { L"VK_SHIFT", 0x10, L"VK_SHIFT" },
    { L"VK_CONTROL", 0x11, L"VK_CONTROL" },
    { L"VK_MENU", 0x12, L"VK_MENU" },
    { L"VK_PAUSE", 0x13, L"Pause" },
    { L"VK_CAPITAL", 0x14, L"VK_CAPITAL" },
    { L"VK_KANA", 0x15, L"VK_KANA" },
    { L"VK_HANGUL", 0x15, L"VK_HANGUL" },
    { L"VK_JUNJA", 0x17, L"VK_JUNJA" },
    { L"VK_FINAL", 0x18, L"VK_FINAL" },
    { L"VK_HANJA", 0x19, L"VK_HANJA" },
    { L"VK_KANJI", 0x19, L"VK_KANJI" },
    { L"VK_ESCAPE", 0x1B, L"Esc" },
    { L"VK_CONVERT", 0x1C, L"VK_CONVERT" },
    { L"VK_NONCONVERT", 0x1D, L"VK_NONCONVERT" },
    { L"VK_ACCEPT", 0x1E, L"VK_ACCEPT" },
    { L"VK_MODECHANGE", 0x1F, L"VK_MODECHANGE" },
    { L"VK_SPACE", 0x20, L"Space" },
    { L"VK_PRIOR", 0x21, L"Page Up" },
    { L"VK_NEXT", 0x22, L"Page Down" },
    { L"VK_END", 0x23, L"End" },
    { L"VK_HOME", 0x24, L"Home" },
    { L"VK_LEFT", 0x25, L"Left" },
    { L"VK_UP", 0x26, L"Up" },
    { L"VK_RIGHT", 0x27, L"Right" },
    { L"VK_DOWN", 0x28, L"Down" },
    { L"VK_SELECT", 0x29, L"VK_SELECT" },
    { L"VK_PRINT", 0x2A, L"VK_PRINT" },
    { L"VK_EXECUTE", 0x2B, L"VK_EXECUTE" },
    { L"VK_SNAPSHOT", 0x2C, L"VK_SNAPSHOT" },
    { L"VK_INSERT", 0x2D, L"Insert" },
    { L"VK_DELETE", 0x2E, L"Delete" },
    { L"VK_HELP", 0x2F, L"Help" },
    { L"VK_0", 0x30, L"0" },
    { L"VK_1", 0x31, L"1" },
    { L"VK_2", 0x32, L"2" },
    { L"VK_3", 0x33, L"3" },
    { L"VK_4", 0x34, L"4" },
    { L"VK_5", 0x35, L"5" },
    { L"VK_6", 0x36, L"6" },
    { L"VK_7", 0x37, L"7" },
    { L"VK_8", 0x38, L"8" },
    { L"VK_9", 0x39, L"9" },
    { L"VK_A", 0x41, L"A" },
    { L"VK_B", 0x42, L"B" },
    { L"VK_C", 0x43, L"C" },
    { L"VK_D", 0x44, L"D" },
    { L"VK_E", 0x45, L"E" },
    { L"VK_F", 0x46, L"F" },
    { L"VK_G", 0x47, L"G" },
    { L"VK_H", 0x48, L"H" },
    { L"VK_I", 0x49, L"I" },
    { L"VK_J", 0x4A, L"J" },
    { L"VK_K", 0x4B, L"K" },
    { L"VK_L", 0x4C, L"L" },
    { L"VK_M", 0x4D, L"M" },
    { L"VK_N", 0x4E, L"N" },
    { L"VK_O", 0x4F, L"O" },
    { L"VK_P", 0x50, L"P" },
    { L"VK_Q", 0x51, L"Q" },
    { L"VK_R", 0x52, L"R" },
    { L"VK_S", 0x53, L"S" },
    { L"VK_T", 0x54, L"T" },
    { L"VK_U", 0x55, L"U" },
    { L"VK_V", 0x56, L"V" },
    { L"VK_W", 0x57, L"W" },
    { L"VK_X", 0x58, L"X" },
    { L"VK_Y", 0x59, L"Y" },
    { L"VK_Z", 0x5A, L"Z" },
    { L"VK_LWIN", 0x5B, L"VK_LWIN" },
    { L"VK_RWIN", 0x5C, L"VK_RWIN" },
    { L"VK_APPS", 0x5D, L"VK_APPS" },
    { L"VK_SLEEP", 0x5D, L"VK_SLEEP" },
    { L"VK_NUMPAD0", 0x60, L"Numpad0" },
    { L"VK_NUMPAD1", 0x61, L"Numpad1" },
    { L"VK_NUMPAD2", 0x62, L"Numpad2" },
    { L"VK_NUMPAD3", 0x63, L"Numpad3" },
    { L"VK_NUMPAD4", 0x64, L"Numpad4" },
    { L"VK_NUMPAD5", 0x65, L"Numpad5" },
    { L"VK_NUMPAD6", 0x66, L"Numpad6" },
    { L"VK_NUMPAD7", 0x67, L"Numpad7" },
    { L"VK_NUMPAD8", 0x68, L"Numpad8" },
    { L"VK_NUMPAD9", 0x69, L"Numpad9" },
    { L"VK_MULTIPLY", 0x6A, L"*" },
    { L"VK_ADD", 0x6B, L"+" },
    { L"VK_SEPARATOR", 0x6C, L"" },
    { L"VK_SUBTRACT", 0x6D, L"-" },
    { L"VK_DECIMAL", 0x6E, L"." },
    { L"VK_DIVIDE", 0x6F, L"/" },
    { L"VK_F1", 0x70, L"F1" },
    { L"VK_F2", 0x71, L"F2" },
    { L"VK_F3", 0x72, L"F3" },
    { L"VK_F4", 0x73, L"F4" },
    { L"VK_F5", 0x74, L"F5" },
    { L"VK_F6", 0x75, L"F6" },
    { L"VK_F7", 0x76, L"F7" },
    { L"VK_F8", 0x77, L"F8" },
    { L"VK_F9", 0x78, L"F9" },
    { L"VK_F10", 0x79, L"F10" },
    { L"VK_F11", 0x7A, L"F11" },
    { L"VK_F12", 0x7B, L"F12" },
    { L"VK_F13", 0x7C, L"F13" },
    { L"VK_F14", 0x7D, L"F14" },
    { L"VK_F15", 0x7E, L"F15" },
    { L"VK_F16", 0x7F, L"F16" },
    { L"VK_F17", 0x80, L"F17" },
    { L"VK_F18", 0x81, L"F18" },
    { L"VK_F19", 0x82, L"F19" },
    { L"VK_F20", 0x83, L"F20" },
    { L"VK_F21", 0x84, L"F21" },
    { L"VK_F22", 0x85, L"F22" },
    { L"VK_F23", 0x86, L"F23" },
    { L"VK_F24", 0x87, L"F24" },
    { L"VK_NUMLOCK", 0x90, L"Numlock" },
    { L"VK_SCROLL", 0x91, L"VK_SCROLL" },
    { L"VK_LSHIFT", 0xA0, L"VK_LSHIFT" },
    { L"VK_RSHIFT", 0xA1, L"VK_RSHIFT" },
    { L"VK_LCONTROL", 0xA2, L"VK_LCONTROL" },
    { L"VK_RCONTROL", 0xA3, L"VK_RCONTROL" },
    { L"VK_LMENU", 0xA4, L"VK_LMENU" },
    { L"VK_RMENU", 0xA5, L"VK_RMENU" },
    { L"VK_BROWSER_BACK", 0xA6, L"" },
    { L"VK_BROWSER_FORWARD", 0xA7, L"" },
    { L"VK_BROWSER_REFRESH", 0xA8, L"" },
    { L"VK_BROWSER_STOP", 0xA9, L"" },
    { L"VK_BROWSER_SEARCH", 0xAA, L"" },
    { L"VK_BROWSER_FAVORITES", 0xAB, L"" },
    { L"VK_BROWSER_HOME", 0xAC, L"" },
    { L"VK_VOLUME_MUTE", 0xAD, L"" },
    { L"VK_VOLUME_DOWN", 0xAE, L"" },
    { L"VK_VOLUME_UP", 0xAF, L"" },
    { L"VK_MEDIA_NEXT_TRACK", 0xB0, L"" },
    { L"VK_MEDIA_PREV_TRACK", 0xB1, L"" },
    { L"VK_MEDIA_STOP", 0xB2, L"" },
    { L"VK_MEDIA_PLAY_PAUSE", 0xB3, L"" },
    { L"VK_LAUNCH_MAIL", 0xB4, L"" },
    { L"VK_LAUNCH_MEDIA_SELECT", 0xB5, L"" },
    { L"VK_LAUNCH_APP1", 0xB6, L"" },
    { L"VK_LAUNCH_APP2", 0xB7, L"" },
    { L"VK_OEM_1 (;:)", 0xBA, L"" },
    { L"VK_OEM_PLUS", 0xBB, L"+" },
    { L"VK_OEM_COMMA", 0xBC, L"" },
    { L"VK_OEM_MINUS", 0xBD, L"-" },
    { L"VK_OEM_PERIOD", 0xBE, L"." },
    { L"VK_OEM_2 (/?)", 0xBF, L"" },
    { L"VK_OEM_3 (`~)", 0xC0, L"~" },
    { L"VK_ATTN", 0xF6, L"" },
    { L"VK_CRSEL", 0xF7, L"" },
    { L"VK_EXSEL", 0xF8, L"" },
    { L"VK_EREOF", 0xF9, L"" },
    { L"VK_PLAY", 0xFA, L"" },
    { L"VK_ZOOM", 0xFB, L"" },
    { L"VK_NONAME", 0xFC, L"" },
    { L"VK_PA1", 0xFD, L"" },
    { L"VK_OEM_CLEAR", 0xFE, L"" }
};

CMenuShortCutKey::CMenuShortCutKey(WORD key, bool bCtrl, bool bAlt, bool bShift, ACCESS_MODE AccessMode, bool bUserAdded, bool bInactive) :
    m_key(key),
    m_bCtrl(bCtrl),
    m_bAlt(bAlt),
    m_bShift(bShift),
    m_AccessMode(AccessMode),
    m_bUserAdded(bUserAdded),
    m_bInactive(bInactive)
{
    m_ShortCutName = L"";
    for (int i = 0, n = sizeof(m_VirtualKeyList) / sizeof(m_VirtualKeyList[0]); i < n; i++)
    {
        if (key == m_VirtualKeyList[i].Key)
        {
            m_ShortCutName = m_VirtualKeyList[i].KeyName;
            break;
        }
    }
    stdstr ShortCutName = stdstr().FromUTF16(m_ShortCutName.c_str());
    if (m_bShift) { ShortCutName = stdstr_f("Shift+%s", ShortCutName.c_str()); }
    if (m_bCtrl) { ShortCutName = stdstr_f("Ctrl+%s", ShortCutName.c_str()); }
    if (m_bAlt) { ShortCutName = stdstr_f("Alt+%s", ShortCutName.c_str()); }
    m_ShortCutName = ShortCutName.ToUTF16();
}

VIRTUAL_KEY * CMenuShortCutKey::VirtualKeyList(int &Size)
{
    Size = sizeof(m_VirtualKeyList) / sizeof(m_VirtualKeyList[0]);
    return (VIRTUAL_KEY *)m_VirtualKeyList;
}

CMenuShortCutKey::RUNNING_STATE CMenuShortCutKey::RunningState(void)
{
    if (g_Settings->LoadBool(GameRunning_CPU_Running))
    {
        return UISettingsLoadBool(UserInterface_InFullScreen) ? CMenuShortCutKey::RUNNING_STATE_FULLSCREEN : CMenuShortCutKey::RUNNING_STATE_WINDOWED;
    }
    return RUNNING_STATE_NOT_RUNNING;
}

bool CMenuShortCutKey::Match(WORD key, bool bCtrl, bool bAlt, bool bShift, RUNNING_STATE RunningState) const
{
    if (key != m_key) { return false; }
    if (bShift != m_bShift) { return false; }
    if (bCtrl != m_bCtrl) { return false; }
    if (bAlt != m_bAlt) { return false; }
    if (!Active(RunningState)) { return false; }
    return true;
}

bool CMenuShortCutKey::Active(RUNNING_STATE RunningState) const
{
    switch (RunningState)
    {
    case CMenuShortCutKey::RUNNING_STATE_NOT_RUNNING:
        if (m_AccessMode == CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING ||
            m_AccessMode == CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN ||
            m_AccessMode == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    case CMenuShortCutKey::RUNNING_STATE_WINDOWED:
        if (m_AccessMode == CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW ||
            m_AccessMode == CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN ||
            m_AccessMode == CMenuShortCutKey::ACCESS_GAME_RUNNING ||
            m_AccessMode == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    case CMenuShortCutKey::RUNNING_STATE_FULLSCREEN:
        if (m_AccessMode == CMenuShortCutKey::ACCESS_GAME_RUNNING_FULLSCREEN ||
            m_AccessMode == CMenuShortCutKey::ACCESS_GAME_RUNNING ||
            m_AccessMode == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    default:
        g_Notify->BreakPoint(__FILE__, __LINE__);
    }
    return false;
}

CShortCutItem::CShortCutItem(LanguageStringID Section, LanguageStringID Title, ACCESS_MODE Access)
{
    Reset(Section, Title, Access);
}

void CShortCutItem::Reset(LanguageStringID Section, LanguageStringID Title, ACCESS_MODE Access)
{
    m_Section = Section;
    m_Title = Title;
    m_Access = Access;
}

void CShortCutItem::AddShortCut(WORD key, bool bCtrl, bool bAlt, bool bShift, ACCESS_MODE AccessMode, bool bUserAdded, bool bInactive)
{
    m_AccelList.push_back(CMenuShortCutKey(key, bCtrl, bAlt, bShift, AccessMode, bUserAdded, bInactive));
}

void CShortCutItem::RemoveItem(CMenuShortCutKey * ShortCut)
{
    if (ShortCut->UserAdded())
    {
        for (SHORTCUT_KEY_LIST::iterator item = m_AccelList.begin(); item != m_AccelList.end(); item++)
        {
            if (ShortCut == &*item)
            {
                m_AccelList.erase(item);
                break;
            }
        }
    }
    else
    {
        ShortCut->SetInactive(true);
    }
}

bool CShortCutItem::Avaliable(CMenuShortCutKey::RUNNING_STATE RunningState) const
{
    switch (RunningState)
    {
    case CMenuShortCutKey::RUNNING_STATE_NOT_RUNNING:
        if (m_Access == CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING ||
            m_Access == CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN ||
            m_Access == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    case CMenuShortCutKey::RUNNING_STATE_WINDOWED:
        if (m_Access == CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW ||
            m_Access == CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN ||
            m_Access == CMenuShortCutKey::ACCESS_GAME_RUNNING ||
            m_Access == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    case CMenuShortCutKey::RUNNING_STATE_FULLSCREEN:
        if (m_Access == CMenuShortCutKey::ACCESS_GAME_RUNNING_FULLSCREEN ||
            m_Access == CMenuShortCutKey::ACCESS_GAME_RUNNING ||
            m_Access == CMenuShortCutKey::ACCESS_ANYTIME)
        {
            return true;
        }
        break;
    default:
        g_Notify->BreakPoint(__FILE__, __LINE__);
    }
    return false;
}

CShortCuts::CShortCuts()
{
    Load();
}

CShortCuts::~CShortCuts()
{
}

std::wstring CShortCuts::ShortCutString(int MenuID, CMenuShortCutKey::RUNNING_STATE RunningState)
{
    CGuard CS(m_CS);

    MSC_MAP::iterator MenuItem = m_ShortCuts.find(MenuID);
    if (MenuItem == m_ShortCuts.end()) { return L""; }

    const SHORTCUT_KEY_LIST & ShortCutList = MenuItem->second.GetAccelItems();
    for (SHORTCUT_KEY_LIST::const_iterator item = ShortCutList.begin(); item != ShortCutList.end(); item++)
    {
        if (!item->Active(RunningState) || item->Inactive())
        {
            continue;
        }
        return item->Name();
    }
    return L"";
}

LanguageStringID CShortCuts::GetMenuItemName(WORD key, bool bCtrl, bool bAlt, bool bShift, RUNNING_STATE RunningState)
{
    CGuard CS(m_CS);

    for (MSC_MAP::iterator Item = m_ShortCuts.begin(); Item != m_ShortCuts.end(); Item++)
    {
        CShortCutItem & short_cut = Item->second;

        for (SHORTCUT_KEY_LIST::const_iterator AccelItem = short_cut.GetAccelItems().begin(); AccelItem != short_cut.GetAccelItems().end(); AccelItem++)
        {
            if (AccelItem->Inactive()) { continue; }
            if (!AccelItem->Match(key, bCtrl, bAlt, bShift, RunningState)) { continue; }
            return short_cut.Title();
        }
    }
    return EMPTY_STRING;
}

void CShortCuts::AddShortCut(WORD ID, LanguageStringID Section, LanguageStringID LangID, CMenuShortCutKey::ACCESS_MODE AccessMode)
{
    m_ShortCuts.insert(MSC_MAP::value_type(ID, CShortCutItem(Section, LangID, AccessMode)));
}

void CShortCuts::Load(bool InitialValues)
{
    CGuard CS(m_CS);

    m_ShortCuts.clear();

    AddShortCut(ID_FILE_OPEN_ROM, STR_SHORTCUT_FILEMENU, MENU_OPEN, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_FILE_OPEN_COMBO, STR_SHORTCUT_FILEMENU, MENU_OPEN_COMBO, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_FILE_MOUNT_SDCARD, STR_SHORTCUT_FILEMENU, MENU_MOUNT_SDCARD, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_FILE_ROM_INFO, STR_SHORTCUT_FILEMENU, MENU_ROM_INFO, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_FILE_STARTEMULATION, STR_SHORTCUT_FILEMENU, MENU_START, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_FILE_ENDEMULATION, STR_SHORTCUT_FILEMENU, MENU_END, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_FILE_ROMDIRECTORY, STR_SHORTCUT_FILEMENU, MENU_CHOOSE_ROM, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
    AddShortCut(ID_FILE_REFRESHROMLIST, STR_SHORTCUT_FILEMENU, MENU_REFRESH, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
    AddShortCut(ID_FILE_EXIT, STR_SHORTCUT_FILEMENU, MENU_EXIT, CMenuShortCutKey::ACCESS_ANYTIME);

    AddShortCut(ID_SYSTEM_RESET_SOFT, STR_SHORTCUT_SYSTEMMENU, MENU_RESET_SOFT, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_RESET_HARD, STR_SHORTCUT_SYSTEMMENU, MENU_RESET_HARD, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_PAUSE, STR_SHORTCUT_SYSTEMMENU, MENU_PAUSE, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_BITMAP, STR_SHORTCUT_SYSTEMMENU, MENU_BITMAP, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_LIMITFPS, STR_SHORTCUT_SYSTEMMENU, MENU_LIMIT_FPS, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_SWAPDISK, STR_SHORTCUT_SYSTEMMENU, MENU_SWAPDISK, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
    AddShortCut(ID_SYSTEM_SAVE, STR_SHORTCUT_SYSTEMMENU, MENU_SAVE, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_SAVEAS, STR_SHORTCUT_SYSTEMMENU, MENU_SAVE_AS, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
    AddShortCut(ID_SYSTEM_RESTORE, STR_SHORTCUT_SYSTEMMENU, MENU_RESTORE, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_SYSTEM_LOAD, STR_SHORTCUT_SYSTEMMENU, MENU_LOAD, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
    AddShortCut(ID_SYSTEM_CHEAT, STR_SHORTCUT_SYSTEMMENU, MENU_CHEAT, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_SYSTEM_GSBUTTON, STR_SHORTCUT_SYSTEMMENU, MENU_GS_BUTTON, CMenuShortCutKey::ACCESS_GAME_RUNNING);

    AddShortCut(ID_OPTIONS_DISPLAY_FR, STR_SHORTCUT_OPTIONS, OPTION_DISPLAY_FR, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_OPTIONS_CHANGE_FR, STR_SHORTCUT_OPTIONS, OPTION_CHANGE_FR, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_OPTIONS_INCREASE_SPEED, STR_SHORTCUT_OPTIONS, STR_INSREASE_SPEED, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_OPTIONS_DECREASE_SPEED, STR_SHORTCUT_OPTIONS, STR_DECREASE_SPEED, CMenuShortCutKey::ACCESS_GAME_RUNNING);

    AddShortCut(ID_CURRENT_SAVE_DEFAULT, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_DEFAULT, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_1, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_1, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_2, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_2, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_3, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_3, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_4, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_4, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_5, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_5, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_6, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_6, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_7, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_7, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_8, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_8, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_9, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_9, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_10, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_10, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_11, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_11, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_12, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_12, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_13, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_13, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_14, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_14, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_15, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_15, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_16, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_16, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_17, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_17, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_18, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_18, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_19, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_19, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_20, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_20, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_21, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_21, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_22, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_22, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_23, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_23, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_24, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_24, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_25, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_25, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_26, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_26, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_27, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_27, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_28, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_28, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_29, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_29, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_30, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_30, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_31, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_31, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_32, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_32, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_33, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_33, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_34, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_34, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_35, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_35, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_36, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_36, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_37, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_37, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_38, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_38, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_39, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_39, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_40, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_40, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_41, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_41, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_42, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_42, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_43, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_43, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_44, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_44, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_45, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_45, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_46, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_46, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_47, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_47, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_48, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_48, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_49, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_49, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_50, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_50, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_51, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_51, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_52, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_52, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_53, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_53, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_54, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_54, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_55, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_55, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_56, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_56, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_57, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_57, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_58, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_58, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_59, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_59, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_60, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_60, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_61, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_61, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_62, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_62, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_63, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_63, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_64, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_64, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_65, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_65, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_66, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_66, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_67, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_67, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_68, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_68, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_69, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_69, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_70, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_70, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_71, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_71, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_72, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_72, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_73, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_73, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_74, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_74, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_75, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_75, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_76, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_76, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_77, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_77, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_78, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_78, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_79, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_79, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_80, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_80, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_81, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_81, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_82, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_82, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_83, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_83, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_84, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_84, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_85, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_85, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_86, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_86, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_87, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_87, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_88, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_88, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_89, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_89, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_90, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_90, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_91, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_91, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_92, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_92, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_93, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_93, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_94, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_94, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_95, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_95, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_96, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_96, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_97, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_97, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_98, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_98, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_CURRENT_SAVE_99, STR_SHORTCUT_SAVESLOT, SAVE_SLOT_99, CMenuShortCutKey::ACCESS_GAME_RUNNING);

    // Option menu
    AddShortCut(ID_OPTIONS_FULLSCREEN, STR_SHORTCUT_OPTIONS, MENU_FULL_SCREEN, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_OPTIONS_ALWAYSONTOP, STR_SHORTCUT_OPTIONS, MENU_ON_TOP, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_CONFIG_GFX, STR_SHORTCUT_OPTIONS, MENU_CONFG_GFX, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_CONFIG_AUDIO, STR_SHORTCUT_OPTIONS, MENU_CONFG_AUDIO, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_CONFIG_CONT, STR_SHORTCUT_OPTIONS, MENU_CONFG_CTRL, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_CONFIG_RSP, STR_SHORTCUT_OPTIONS, MENU_CONFG_RSP, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_CPU_USAGE, STR_SHORTCUT_OPTIONS, MENU_SHOW_CPU, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    AddShortCut(ID_OPTIONS_SIMPLE_SETTINGS, STR_SHORTCUT_OPTIONS, MENU_SIMPLE_SETTINGS, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);
    AddShortCut(ID_OPTIONS_SETTINGS, STR_SHORTCUT_OPTIONS, MENU_SETTINGS, CMenuShortCutKey::ACCESS_NOT_IN_FULLSCREEN);

    CPath ShortCutFile = UISettingsLoadStringVal(SupportFile_ShortCuts);
    if (!ShortCutFile.Exists() || InitialValues)
    {
        m_ShortCuts.find(ID_FILE_OPEN_ROM)->second.AddShortCut('O', TRUE, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_FILE_OPEN_ROM)->second.AddShortCut('O', TRUE, false, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_FILE_OPEN_COMBO)->second.AddShortCut('O', TRUE, false, TRUE, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_FILE_OPEN_COMBO)->second.AddShortCut('O', TRUE, false, TRUE, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_FILE_STARTEMULATION)->second.AddShortCut(VK_F11, false, false, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_FILE_ENDEMULATION)->second.AddShortCut(VK_F12, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_FILE_REFRESHROMLIST)->second.AddShortCut(VK_F5, false, false, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_FILE_EXIT)->second.AddShortCut(VK_F4, false, true, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_FILE_EXIT)->second.AddShortCut(VK_F4, false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_DEFAULT)->second.AddShortCut(0xC0, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_1)->second.AddShortCut('1', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_2)->second.AddShortCut('2', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_3)->second.AddShortCut('3', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_4)->second.AddShortCut('4', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_5)->second.AddShortCut('5', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_6)->second.AddShortCut('6', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_7)->second.AddShortCut('7', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_8)->second.AddShortCut('8', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_9)->second.AddShortCut('9', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_10)->second.AddShortCut('0', false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_11)->second.AddShortCut('1', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_12)->second.AddShortCut('2', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_13)->second.AddShortCut('3', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_14)->second.AddShortCut('4', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_15)->second.AddShortCut('5', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_16)->second.AddShortCut('6', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_17)->second.AddShortCut('7', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_18)->second.AddShortCut('8', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_19)->second.AddShortCut('9', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_20)->second.AddShortCut('0', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_21)->second.AddShortCut('1', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_22)->second.AddShortCut('2', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_23)->second.AddShortCut('3', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_24)->second.AddShortCut('4', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_25)->second.AddShortCut('5', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_26)->second.AddShortCut('6', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_27)->second.AddShortCut('7', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_28)->second.AddShortCut('8', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_29)->second.AddShortCut('9', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_30)->second.AddShortCut('0', false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_31)->second.AddShortCut('1', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_32)->second.AddShortCut('2', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_33)->second.AddShortCut('3', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_34)->second.AddShortCut('4', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_35)->second.AddShortCut('5', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_36)->second.AddShortCut('6', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_37)->second.AddShortCut('7', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_38)->second.AddShortCut('8', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_39)->second.AddShortCut('9', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_40)->second.AddShortCut('0', false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_41)->second.AddShortCut('1', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_42)->second.AddShortCut('2', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_43)->second.AddShortCut('3', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_44)->second.AddShortCut('4', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_45)->second.AddShortCut('5', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_46)->second.AddShortCut('6', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_47)->second.AddShortCut('7', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_48)->second.AddShortCut('8', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_49)->second.AddShortCut('9', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_50)->second.AddShortCut('0', true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_51)->second.AddShortCut('1', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_52)->second.AddShortCut('2', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_53)->second.AddShortCut('3', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_54)->second.AddShortCut('4', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_55)->second.AddShortCut('5', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_56)->second.AddShortCut('6', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_57)->second.AddShortCut('7', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_58)->second.AddShortCut('8', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_59)->second.AddShortCut('9', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_60)->second.AddShortCut('0', true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_61)->second.AddShortCut('1', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_62)->second.AddShortCut('2', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_63)->second.AddShortCut('3', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_64)->second.AddShortCut('4', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_65)->second.AddShortCut('5', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_66)->second.AddShortCut('6', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_67)->second.AddShortCut('7', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_68)->second.AddShortCut('8', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_69)->second.AddShortCut('9', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_70)->second.AddShortCut('0', false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_71)->second.AddShortCut('1', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_72)->second.AddShortCut('2', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_73)->second.AddShortCut('3', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_74)->second.AddShortCut('4', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_75)->second.AddShortCut('5', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_76)->second.AddShortCut('6', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_77)->second.AddShortCut('7', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_78)->second.AddShortCut('8', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_79)->second.AddShortCut('9', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_80)->second.AddShortCut('0', true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_CURRENT_SAVE_93)->second.AddShortCut(0xC0, true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_94)->second.AddShortCut(0xC0, false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_95)->second.AddShortCut(0xC0, false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_96)->second.AddShortCut(0xC0, true, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_97)->second.AddShortCut(0xC0, true, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_98)->second.AddShortCut(0xC0, false, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_CURRENT_SAVE_99)->second.AddShortCut(0xC0, true, true, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);

        m_ShortCuts.find(ID_OPTIONS_FULLSCREEN)->second.AddShortCut(VK_RETURN, false, true, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_FULLSCREEN)->second.AddShortCut(VK_ESCAPE, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_ALWAYSONTOP)->second.AddShortCut('A', true, false, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_ALWAYSONTOP)->second.AddShortCut('A', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_OPTIONS_SETTINGS)->second.AddShortCut('T', true, false, false, CMenuShortCutKey::ACCESS_GAME_NOT_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_SETTINGS)->second.AddShortCut('T', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_SYSTEM_RESET_SOFT)->second.AddShortCut(VK_F1, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_RESET_HARD)->second.AddShortCut(VK_F1, false, false, true, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_PAUSE)->second.AddShortCut(VK_F2, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_PAUSE)->second.AddShortCut(VK_PAUSE, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_BITMAP)->second.AddShortCut(VK_F3, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_LIMITFPS)->second.AddShortCut(VK_F4, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_SWAPDISK)->second.AddShortCut('D', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_SYSTEM_SAVE)->second.AddShortCut(VK_F5, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_RESTORE)->second.AddShortCut(VK_F7, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_SYSTEM_LOAD)->second.AddShortCut('L', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_SYSTEM_SAVEAS)->second.AddShortCut('S', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_SYSTEM_CHEAT)->second.AddShortCut('C', true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING_WINDOW);
        m_ShortCuts.find(ID_SYSTEM_GSBUTTON)->second.AddShortCut(VK_F9, false, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_INCREASE_SPEED)->second.AddShortCut(VK_OEM_PLUS, true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
        m_ShortCuts.find(ID_OPTIONS_DECREASE_SPEED)->second.AddShortCut(VK_OEM_MINUS, true, false, false, CMenuShortCutKey::ACCESS_GAME_RUNNING);
    }
    else
    {
        CMenuShortCutKey::ACCESS_MODE AccessMode;
        int ID, key, bCtrl, bAlt, bShift, bUserAdded, bInactive;

        FILE *file = fopen(ShortCutFile, "r");
        if (file)
        {
            do
            {
                char Line[300];
                if (fgets(Line, sizeof(Line), file) != nullptr)
                {
                    sscanf(Line, "%d,%d,%d,%d,%d,%d,%d,%d", &ID, &key, &bCtrl, &bAlt, &bShift, &AccessMode,
                        &bUserAdded, &bInactive);

                    MSC_MAP::iterator item = m_ShortCuts.find(ID);
                    if (item == m_ShortCuts.end()) { continue; }
                    item->second.AddShortCut((WORD)(key & 0xFFFF), bCtrl == 1, bAlt == 1, bShift == 1, AccessMode, bUserAdded == 1, bInactive == 1);
                }
            } while (feof(file) == 0);
            fclose(file);
        }
    }
}

void CShortCuts::Save(void)
{
    CGuard CS(m_CS);

    stdstr FileName = UISettingsLoadStringVal(SupportFile_ShortCuts);
    FILE *file = fopen(FileName.c_str(), "w");
    if (file == nullptr)
    {
        return;
    }

    for (MSC_MAP::iterator Item = m_ShortCuts.begin(); Item != m_ShortCuts.end(); Item++)
    {
        for (SHORTCUT_KEY_LIST::const_iterator ShortCut = Item->second.GetAccelItems().begin(); ShortCut != Item->second.GetAccelItems().end(); ShortCut++)
        {
            fprintf(file, "%d,%d,%d,%d,%d,%d,%d,%d\n",
                Item->first,
                ShortCut->Key(),
                ShortCut->Ctrl(),
                ShortCut->Alt(),
                ShortCut->Shift(),
                ShortCut->AccessMode(),
                ShortCut->UserAdded(),
                ShortCut->Inactive());
        }
    }
    fclose(file);
}

HACCEL CShortCuts::GetAcceleratorTable(void)
{
    CGuard CS(m_CS);

    // Generate an ACCEL list
    int size = 0, MaxSize = m_ShortCuts.size() * 5;
    ACCEL * AccelList = new ACCEL[MaxSize];
    CMenuShortCutKey::RUNNING_STATE RunningState = CMenuShortCutKey::RunningState();

    for (MSC_MAP::iterator Item = m_ShortCuts.begin(); Item != m_ShortCuts.end(); Item++)
    {
        CShortCutItem & short_cut = Item->second;
        if (!short_cut.Avaliable(RunningState))
        {
            continue;
        }

        SHORTCUT_KEY_LIST ShortCutAccelList = short_cut.GetAccelItems();
        for (SHORTCUT_KEY_LIST::iterator AccelIter = ShortCutAccelList.begin(); AccelIter != ShortCutAccelList.end(); AccelIter++)
        {
            CMenuShortCutKey & Key = *AccelIter;
            if (Key.Inactive())
            {
                continue;
            }
            if (!Key.Active(RunningState))
            {
                continue;
            }
            if (size >= MaxSize) { break; }
            AccelList[size].cmd = (WORD)Item->first;
            AccelList[size].key = Key.Key();
            AccelList[size].fVirt = FVIRTKEY;
            if (Key.Alt()) { AccelList[size].fVirt |= FALT; }
            if (Key.Ctrl()) { AccelList[size].fVirt |= FCONTROL; }
            if (Key.Shift()) { AccelList[size].fVirt |= FSHIFT; }
            size += 1;
        }
    }

    WriteTrace(TraceUserInterface, TraceDebug, "CreateAcceleratorTable");
    HACCEL AccelTable = CreateAcceleratorTable(AccelList, size);
    WriteTrace(TraceUserInterface, TraceDebug, "Delete accel list");
    delete[] AccelList;
    return AccelTable;
}
