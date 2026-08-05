#pragma once

#include <Common/Path.h>
#include <Project64-core/Settings/SettingType/SettingsType-Base.h>

class CSettingAppDataEnabledConfig :
    public CSettingType
{
public:
    CSettingAppDataEnabledConfig(const char * appDataPath, const char * Name, bool DefaultValue);
    virtual ~CSettingAppDataEnabledConfig();

    virtual bool IndexBasedSetting(void) const { return false; }
    virtual SettingType GetSettingType(void) const { return SettingType_Unknown; }
    virtual bool IsSettingSet(void) const;

    // Return the values
    virtual bool Load(uint32_t Index, bool & Value) const;
    virtual bool Load(uint32_t Index, uint32_t & Value) const;
    virtual bool Load(uint32_t Index, std::string & Value) const;

    // Return the default values
    virtual void LoadDefault(uint32_t Index, bool & Value) const;
    virtual void LoadDefault(uint32_t Index, uint32_t & Value) const;
    virtual void LoadDefault(uint32_t Index, std::string & Value) const;

    // Update the settings
    virtual void Save(uint32_t Index, bool Value);
    virtual void Save(uint32_t Index, uint32_t Value);
    virtual void Save(uint32_t Index, const std::string & Value);
    virtual void Save(uint32_t Index, const char * Value);

    // Delete the setting
    virtual void Delete(uint32_t Index);

protected:
    const char * m_DefaultStr;
    const uint32_t m_DefaultValue;
    std::string m_UserPath;
    CPath m_AppdataCfgPath;

private:
    CSettingAppDataEnabledConfig(const CSettingAppDataEnabledConfig&);
    CSettingAppDataEnabledConfig& operator=(const CSettingAppDataEnabledConfig&);
};
