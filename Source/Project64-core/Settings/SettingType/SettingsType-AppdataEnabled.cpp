#include "stdafx.h"
#include "SettingsType-AppdataEnabled.h"
#include <Common/path.h>

#include <io.h>

CSettingAppDataEnabledConfig::CSettingAppDataEnabledConfig(const char * path, const char * Name, bool DefaultValue) :
    m_DefaultStr(Name),
    m_DefaultValue(DefaultValue),
    m_UserPath(path),
	m_AppdataCfgPath(path, "enabled.cfg")
{
}

CSettingAppDataEnabledConfig::~CSettingAppDataEnabledConfig()
{
}

bool CSettingAppDataEnabledConfig::IsSettingSet(void) const
{
	return m_AppdataCfgPath.Exists();
}

bool CSettingAppDataEnabledConfig::Load(uint32_t Index, bool & Value) const
{
    Value = IsSettingSet();
    return true;
}

bool CSettingAppDataEnabledConfig::Load(uint32_t /*Index*/, uint32_t & Value) const
{
    Value = IsSettingSet();
    return true;
}

bool CSettingAppDataEnabledConfig::Load(uint32_t Index, std::string & Value) const
{
	g_Notify->BreakPoint(__FILE__, __LINE__);
    return false;
}

// Return the default values
void CSettingAppDataEnabledConfig::LoadDefault(uint32_t Index, bool & Value) const
{
    Value = false;
}

void CSettingAppDataEnabledConfig::LoadDefault(uint32_t /*Index*/, uint32_t & Value) const
{
    Value = false;
}

void CSettingAppDataEnabledConfig::LoadDefault(uint32_t /*Index*/, std::string & Value) const
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

// Update the settings
void CSettingAppDataEnabledConfig::Save(uint32_t Index, bool Value)
{
    if (Value)
    {
        CreateDirectoryA(m_UserPath.c_str(), nullptr);
        HANDLE hndl = CreateFileA(m_AppdataCfgPath, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		CloseHandle(hndl);
    }
    else
    {
        m_AppdataCfgPath.Delete();
    }
}

void CSettingAppDataEnabledConfig::Save(uint32_t /*Index*/, uint32_t Value)
{
	Save(0, Value != 0);
}

void CSettingAppDataEnabledConfig::Save(uint32_t Index, const std::string & Value)
{
	g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingAppDataEnabledConfig::Save(uint32_t /*Index*/, const char * Value)
{
    g_Notify->BreakPoint(__FILE__, __LINE__);
}

void CSettingAppDataEnabledConfig::Delete(uint32_t Index)
{
    m_AppdataCfgPath.Delete();
}
