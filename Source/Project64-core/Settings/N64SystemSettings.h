#pragma once

class CN64SystemSettings
{
protected:
    CN64SystemSettings();
    virtual ~CN64SystemSettings();

    inline static bool bBasicMode(void) { return m_bBasicMode; }
    inline static bool bDisplayFrameRate(void) { return m_bDisplayFrameRate; }
    inline static bool bShowCPUPer(void) { return m_bShowCPUPer; }
    inline static bool bShowDListAListCount(void) { return m_bShowDListAListCount; }
    inline static bool bLimitFPS(void) { return m_bLimitFPS; }
    inline static int bInputDelay(void) { return m_bInputDelay; }

private:
    static void CALL RefreshSettings(void *);

    static bool m_bShowCPUPer;
    static bool m_bBasicMode;
    static bool m_bLimitFPS;
    static bool m_bShowDListAListCount;
    static bool m_bDisplayFrameRate;
    static int m_bInputDelay;

    static int32_t     m_RefCount;
};
