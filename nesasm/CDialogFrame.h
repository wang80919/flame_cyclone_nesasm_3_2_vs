#pragma once
#include "CWindow/CDialogBase.h"
#include "Win32Utils/CIniHelper.h"
#include <thread>
#include <atomic>
#include <vector>
#include <string>

#define CONFIG_REG_KEY                                      HKCU

#define CONFIG_ROOT_PATH                                    _T("Software\\NESASM_3_2")
#define CONFIG_INPUT_PATH                                   _T("Intput")
#define CONFIG_OUTPUT_PATH                                  _T("Output")
#define CONFIG_RAW_ENABLE                                   _T("Raw")
#define CONFIG_FNS_ENABLE                                   _T("Fns")
#define CONFIG_FILL_BYTE                                    _T("Fill")
#define CONFIG_BANK_SIZE                                    _T("Bank")
#define CONFIG_SEG_ENABLE                                   _T("Seg")
#define CONFIG_SEG_DETAIL_ENABLE                            _T("Seg_Detail")
#define CONFIG_ZP_OPT_ENABLE                                _T("Zp_opt")
#define CONFIG_TOPMOST_ENABLE                               _T("Topmost")
#define CONFIG_LANGUAGE                                     _T("Lang")

#define HELP_ABOUT_URL                                      _T("https://gitee.com/flame_cyclone/nesasm_3_2_vs")

class CDialogFrame :
    public CDialogBase
{
public:

    virtual BOOL PreTranslateMessage(LPMSG pMsg);

protected:

    dlg_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
	dlg_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnCompileDone(WPARAM wParam, LPARAM lParam);
    dlg_msg LRESULT OnTimer(WPARAM wParam, LPARAM lParam);

    dlg_command LRESULT OnTrayIcon(WPARAM wParam, LPARAM lParam);
    dlg_command LRESULT OnOK(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnCancel(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnTest(WORD wNotify, WORD wID, HWND hWnd);

    dlg_command LRESULT OnCommandExit(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnFileInput(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnFileOutput(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnSettingsTopmost(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnHelpAbout(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnLangChinese(WORD wNotify, WORD wID, HWND hWnd);
    dlg_command LRESULT OnLangEnglish(WORD wNotify, WORD wID, HWND hWnd);

private:
    void LanguageChange(LANGID lang);
    void StartCompile(const std::vector<std::string>& args);
    void FlushOutputToEdit(void);

private:
    HMENU m_hPopMenu = NULL;
    HMENU m_hMainMenu = NULL;
    HWND m_hHideWnd = NULL;
    HWND m_hEditFill = NULL;
    bool m_fCuiMode = false;
    bool m_fCommandLaunch = false;
    bool m_fCompiling = false;
    std::thread m_compileThread;
    int64_t m_tmBegin = 0;
    long m_shownLen = 0;
    CIniHelper m_config;

    DECLARE_DLG_MESSAGE_MAP()
};