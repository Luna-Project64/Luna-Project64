#include "SimplifiedSettings.h"

void CCheatsUI::Display(HWND hParent, bool BlockExecution)
{
    if (BlockExecution)
    {
        m_bModal = true;
        DoModal(hParent);
    }
    else if (m_hWnd != nullptr)
    {
        SetFocus();
    }
    else
    {
        m_bModal = false;
        Create(hParent);
    }
}

LRESULT SimplifiedSettings::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
	SetDlgItemText(IDC_MINIMIZEDSLEEP, wGS(OPTION_MINIMIZED_SLEEP).c_str());
	SetDlgItemText(IDC_DISCORD_RPC, wGS(OPTION_DISCORD_RPC).c_str());
	SetDlgItemText(IDC_INPUTDELAY_TXT, wGS(OPTION_INPUTDELAY).c_str());
	SetDlgItemText(IDC_COUNTFACT_TEXT, wGS(ROM_COUNTER_FACTOR).c_str());
    SetDlgItemText(IDC_OVERCLOCKVI_TEXT, wGS(EMPTY_STRING).c_str());
    SetDlgItemText(IDC_ACCURATEGFX_TEXT, wGS(EMPTY_STRING).c_str());
    SetDlgItemText(IDC_JABOGFX, wGS(EMPTY_STRING).c_str());
    SetDlgItemText(IDC_INTERPRETER, wGS(EMPTY_STRING).c_str());
    SetDlgItemText(IDC_CONT_NAME2, wGS(EMPTY_STRING).c_str());

	CModifiedComboBox* ComboBox;

    ComboBox = AddModComboBox(GetDlgItem(IDC_COUNTFACT), Default_None);
    if (ComboBox)
    {
        ComboBox->SetTextField(GetDlgItem(IDC_COUNTFACT_TEXT));
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), KEEP_CURRENT);
        ComboBox->AddItem(wGS(NUMBER_0).c_str(), 0);
        ComboBox->AddItem(wGS(NUMBER_1).c_str(), 1);
        ComboBox->AddItem(wGS(NUMBER_2).c_str(), 2);
        ComboBox->AddItem(wGS(NUMBER_3).c_str(), 3);
        ComboBox->AddItem(wGS(NUMBER_4).c_str(), 4);
        ComboBox->AddItem(wGS(NUMBER_5).c_str(), 5);
        ComboBox->AddItem(wGS(NUMBER_6).c_str(), 6);
    }

    ComboBox = AddModComboBox(GetDlgItem(IDC_OVERCLOCKVI), Default_None);
    if (ComboBox)
    {
        ComboBox->SetTextField(GetDlgItem(IDC_OVERCLOCKVI_TEXT));
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), KEEP_CURRENT);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), 1500);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), 2200);
    }

    ComboBox = AddModComboBox(GetDlgItem(IDC_ACCURATEGFX), Default_None);
    if (ComboBox)
    {
        ComboBox->SetTextField(GetDlgItem(IDC_ACCURATEGFX));
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), KEEP_CURRENT);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), 0);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), 1);
    }
    
    ComboBox = AddModComboBox(GetDlgItem(CONT_LIST), Default_None);
    if (ComboBox)
    {
        ComboBox->SetTextField(GetDlgItem(IDC_CONT_NAME2));
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), KEEP_CURRENT);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), PLUGIN_NRAGE);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), PLUGIN_LUNADINPUT);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), PLUGIN_LMAPPER);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), PLUGIN_WIIUGCN);
        ComboBox->AddItem(wGS(EMPTY_STRING).c_str(), PLUGIN_RAPHNETRAW);
    }

    AddModCheckBox(GetDlgItem(IDC_MINIMIZEDSLEEP), (SettingID)Setting_MinimizedSleep);
    AddModCheckBox(GetDlgItem(IDC_DISCORD_RPC), (SettingID)Setting_EnableDiscordRPC);
    string CurrentGFXPlugin = g_Settings->LoadStringVal(Plugin_GFX_Current);
    if (CurrentGFXPlugin.find("Jabo") == string::npos) {
        AddModCheckBox(GetDlgItem(IDC_JABOGFX), (SettingID)1);
    }
    else {
        AddModCheckBox(GetDlgItem(IDC_JABOGFX), (SettingID)0);
    }

    if (g_Settings->LoadDword(Game_CpuType) == CPU_Interpreter) {
        if (CurrentGFXPlugin.find("Jabo") == string::npos) {
            AddModCheckBox(GetDlgItem(IDC_INTERPRETER), (SettingID)1);
        }
        else {
            AddModCheckBox(GetDlgItem(IDC_INTERPRETER), (SettingID)0);
        }
    }
}

LRESULT SimplifiedSettings::OnAllGamesCmd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
    
}