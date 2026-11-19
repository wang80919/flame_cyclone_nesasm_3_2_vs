#include "CDialogFrame.h"
#include <Windows.h>
#include <strsafe.h>
#include <thread>
#include <vector>
#include <string>
#include "resource.h"

#include "Win32Utils/CComFileDialog.h"
#include "Win32Utils/CPathUtils.h"
#include "Win32Utils/CTimeUtils.h"
#include "Win32Utils/CStrUtils.h"
#include "Win32Utils/CCommandLine.h"
#include "Win32Utils/CProcessUtils.h"

extern "C"
{
#include "source/defs.h"
#include "source/externs.h"
}

extern int main(int argc, char** argv);

#define WM_APP_COMPILE_DONE    (WM_APP + 0x100)
#define COMPILE_TIMER_ID        (0x101)

BEGIN_DLG_MESSAGE_MAP(CDialogFrame, CDialogBase)
    ON_DLG_MESSAGE(WM_INITDIALOG, &CDialogFrame::OnInitDialog)
    ON_DLG_MESSAGE(WM_CLOSE, &CDialogFrame::OnClose)
    ON_DLG_MESSAGE(WM_DROPFILES, &CDialogFrame::OnDropFiles)
    ON_DLG_MESSAGE(WM_TIMER, &CDialogFrame::OnTimer)
    ON_DLG_MESSAGE(WM_APP_COMPILE_DONE, &CDialogFrame::OnCompileDone)
    ON_DLG_COMMAND(IDOK, &CDialogFrame::OnOK)
    ON_DLG_COMMAND(IDCANCEL, &CDialogFrame::OnCancel)
    ON_DLG_COMMAND(IDC_BUTTON_INPUT, &CDialogFrame::OnFileInput)
    ON_DLG_COMMAND(IDC_BUTTON_OUTPUT, &CDialogFrame::OnFileOutput)
    ON_DLG_COMMAND(ID_FILE_EXIT, &CDialogFrame::OnCommandExit)
    ON_DLG_COMMAND(ID_SETTINGS_TOPMOST, &CDialogFrame::OnSettingsTopmost)
    ON_DLG_COMMAND(ID_HELP_ABOUT, &CDialogFrame::OnHelpAbout)
    ON_DLG_COMMAND(ID_LANGUAGE_CHINESE, &CDialogFrame::OnLangChinese)
    ON_DLG_COMMAND(ID_LANGUAGE_ENGLISH, &CDialogFrame::OnLangEnglish)
END_DLG_MESSAGE_MAP()

LRESULT CDialogFrame::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
    m_hHideWnd = CreateWindow(
        _T("Static"), _T("Hide"), WS_CHILDWINDOW, 0, 0, 0, 0,
        m_hWnd, NULL, GetModuleHandle(NULL), NULL);
    if (m_hHideWnd)
    {
        ::SetParent(m_hHideWnd, NULL);
    }

    m_hMainMenu = ::LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU));
    if (m_hMainMenu)
    {
    }

    m_hEditFill = GetItem(IDC_EDIT_FILL);

    ::SetMenu(m_hWnd, m_hMainMenu);

    //允许低权限进程拖动文件进来
    CHANGEFILTERSTRUCT cfs = { 0 };
    cfs.cbSize = sizeof(cfs);
    ChangeWindowMessageFilterEx(m_hWnd, WM_DROPFILES, MSGFLT_ALLOW, &cfs);
    ChangeWindowMessageFilterEx(m_hWnd, WM_COPYDATA, MSGFLT_ALLOW, &cfs);
    ChangeWindowMessageFilterEx(m_hWnd, WM_COPYDATA - 1, MSGFLT_ALLOW, &cfs);

    // 设置图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON));
    if (hIcon)
    {
        ::SendMessage(m_hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        ::SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }

    SetWindowText(m_hWnd, _T(NESASM_TITLE));
    SetItemString(IDC_EDIT_FILL, _T("FF"));

    std::vector<_tstring> strCmd = CCommandLine::GetCommandList();
    if (strCmd.empty())
    {
        SetItemString(IDC_EDIT_INPUT_FILE, m_config.GetString(_T("config"), CONFIG_INPUT_PATH));
        SetItemString(IDC_EDIT_OUTPUT_FILE, m_config.GetString(_T("config"), CONFIG_OUTPUT_PATH));
        _tstring strBankSize = m_config.GetString(_T("config"), CONFIG_BANK_SIZE);
        if (_T("1") == strBankSize)
        {
            CheckButton(IDC_RADIO_16K);
        }
        else if (_T("2") == strBankSize)
        {
            CheckButton(IDC_RADIO_32K);
        }
        else
        {
            CheckButton(IDC_RADIO_8K);
        }

        SetItemString(IDC_EDIT_FILL, m_config.GetString(_T("config"), CONFIG_FILL_BYTE, _T("FF")));
        CheckButton(IDC_CHECK_RAW, m_config.GetNumber(_T("config"), CONFIG_RAW_ENABLE, 0));
        CheckButton(IDC_CHECK_FNS, m_config.GetNumber(_T("config"), CONFIG_FNS_ENABLE, 0));
        CheckButton(IDC_CHECK_SEG, m_config.GetNumber(_T("config"), CONFIG_SEG_ENABLE, 0));
        CheckButton(IDC_CHECK_SEG_DETAIL, m_config.GetNumber(_T("config"), CONFIG_SEG_DETAIL_ENABLE, 0));

        CheckButton(IDC_CHECK_ZP_OPT, true);
        CheckButton(IDC_CHECK_ZP_OPT, m_config.GetNumber(_T("config"), CONFIG_ZP_OPT_ENABLE, 0));

        ShowWindow(SW_SHOW);
    }
    else
    {
        m_fCommandLaunch = true;
        int nBankSize = 0;
        m_fCuiMode = CProcessUtils::IsWindowsCUI(CProcessUtils::GetParentID(GetCurrentProcessId()));
        if (!m_fCuiMode)
        {
            ShowWindow(SW_SHOW);
        }

        CheckButton(IDC_CHECK_ZP_OPT, true);

        for (int i = 0; i < strCmd.size(); i++)
        {
            if (0 == _tcsicmp(strCmd[i].c_str(), _T("-s")))
            {
                CheckButton(IDC_CHECK_SEG);
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-fill")))
            {
                if (i + 1 < strCmd.size())
                {
                    SetItemString(IDC_EDIT_FILL, strCmd[++i]);
                }
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-zp_opt")))
            {
                if (i + 1 < strCmd.size())
                {
                    CheckButton(IDC_CHECK_ZP_OPT, 1 == _tcstol(strCmd[++i].c_str(), NULL, 16));
                }
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-raw")))
            {
                CheckButton(IDC_CHECK_RAW);
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-fns")))
            {
                CheckButton(IDC_CHECK_FNS);
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-out")))
            {
                if (i + 1 < strCmd.size())
                {
                    SetItemString(IDC_EDIT_OUTPUT_FILE, strCmd[++i]);
                }
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-in")))
            {
                if (i + 1 < strCmd.size())
                {
                    SetItemString(IDC_EDIT_INPUT_FILE, strCmd[++i]);
                }
            }
            else if (0 == _tcsicmp(strCmd[i].c_str(), _T("-bank")))
            {
                if (i + 1 < strCmd.size())
                {
                    nBankSize = _tcstol(strCmd[++i].c_str(), NULL, 16);
                }
            }
        }

        if (1 == nBankSize)
        {
            CheckButton(IDC_RADIO_16K);
        }
        else if (2 == nBankSize)
        {
            CheckButton(IDC_RADIO_32K);
        }
        else
        {
            CheckButton(IDC_RADIO_8K);
        }

        PostCommand(IDOK, 0, 0);
    }

    g_hEditOutput = GetItem(IDC_EDIT_OUTPUT);

    if (m_config.GetNumber(_T("config"), CONFIG_TOPMOST_ENABLE, 0))
    {
        PostCommand(ID_SETTINGS_TOPMOST);
    }

    DWORD dwLang = m_config.GetNumber(_T("config"), CONFIG_LANGUAGE, 0);
    if (2 == dwLang)
    {
        PostCommand(ID_LANGUAGE_CHINESE);
    }
    else if(1 == dwLang)
    {
        PostCommand(ID_LANGUAGE_ENGLISH);
    }
    else
    {
        LANGID langID = GetThreadUILanguage();
        if (LANG_CHINESE == PRIMARYLANGID(langID))
        {
            PostCommand(ID_LANGUAGE_CHINESE);
        }
        else
        {
            PostCommand(ID_LANGUAGE_ENGLISH);
        }
    }

    return TRUE;
}

LRESULT CDialogFrame::OnClose(WPARAM wParam, LPARAM lParam)
{
    // 编译进行中:停止定时器并等待编译线程结束,避免其
    // 继续访问已被销毁的窗口/全局输出缓冲
    if (m_fCompiling)
    {
        ::KillTimer(m_hWnd, COMPILE_TIMER_ID);
        if (m_compileThread.joinable())
        {
            m_compileThread.join();
        }
        m_fCompiling = false;
    }

    if (m_fCommandLaunch)
    {
        return (LRESULT)FALSE;
    }

    if (IsButtonChecked(IDC_RADIO_8K))
    {
        m_config.SetString(_T("config"), CONFIG_BANK_SIZE, _T("0"));
    }
    else if (IsButtonChecked(IDC_RADIO_16K))
    {
        m_config.SetString(_T("config"), CONFIG_BANK_SIZE, _T("1"));
    }
    else if (IsButtonChecked(IDC_RADIO_32K))
    {
        m_config.SetString(_T("config"), CONFIG_BANK_SIZE, _T("2"));
    }

    m_config.SetString(_T("config"), CONFIG_FILL_BYTE, GetItemString(IDC_EDIT_FILL));
    m_config.SetNumber(_T("config"), CONFIG_RAW_ENABLE, IsButtonChecked(IDC_CHECK_RAW));
    m_config.SetNumber(_T("config"), CONFIG_FNS_ENABLE, IsButtonChecked(IDC_CHECK_FNS));
    m_config.SetNumber(_T("config"), CONFIG_SEG_ENABLE, IsButtonChecked(IDC_CHECK_SEG));
    m_config.SetNumber(_T("config"), CONFIG_SEG_DETAIL_ENABLE, IsButtonChecked(IDC_CHECK_SEG_DETAIL));
    m_config.SetNumber(_T("config"), CONFIG_ZP_OPT_ENABLE, IsButtonChecked(IDC_CHECK_ZP_OPT));
    m_config.SetString(_T("config"), CONFIG_INPUT_PATH, GetItemString(IDC_EDIT_INPUT_FILE));
    m_config.SetString(_T("config"), CONFIG_OUTPUT_PATH, GetItemString(IDC_EDIT_OUTPUT_FILE));

    return (LRESULT)FALSE;
}

LRESULT CDialogFrame::OnDropFiles(WPARAM wParam, LPARAM lParam)
{
    HDROP hDrop = (HDROP)wParam;
    TCHAR szFile[MAX_PATH] = { 0 };
    UINT nCount = DragQueryFile(hDrop, (UINT)(-1), szFile, _countof(szFile));

    _tstring strCurDir = CPathUtils::GetCurrentModuleDir();
    for (UINT i = 0; i < nCount; i++)
    {
        ::DragQueryFile(hDrop, i, szFile, _countof(szFile));
        if (_T("asm") == CPathUtils::GetFileExt(szFile))
        {
            SetItemString(IDC_EDIT_INPUT_FILE, szFile);
            m_config.SetString(_T("config"), CONFIG_INPUT_PATH, szFile);
            break;
        }
    }

    DragFinish(hDrop);

    return (LRESULT)TRUE;
}

BOOL CDialogFrame::PreTranslateMessage(LPMSG pMsg)
{
    if (WM_CHAR == pMsg->message && g_hEditOutput == pMsg->hwnd)
    {
        if ((GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000))
        {
            return FALSE;
        }

        return TRUE;
    }

    if (pMsg->hwnd == m_hEditFill)
    {
        if (WM_CHAR == pMsg->message)
        {
            TCHAR ch = (TCHAR)pMsg->wParam;

            if (VK_BACK == ch)
            {
                return FALSE;
            }


            DWORD dwSelBeg = 0;
            DWORD dwSelEnd = 0;

            ::SendMessage(m_hEditFill, EM_GETSEL, (WPARAM)&dwSelBeg, (LPARAM)&dwSelEnd);
            int nLeng = GetWndStringLength(m_hEditFill);
            if (nLeng >= 2 && 0 == (dwSelEnd - dwSelBeg))
            {
                return TRUE;
            }

            if (ch >= _T('0') && ch <= _T('9'))
            {
                return (LRESULT)FALSE;
            }

            if (ch >= _T('a') && ch <= _T('f'))
            {
                return (LRESULT)FALSE;
            }

            return (LRESULT)TRUE;
        }

    }

    return (LRESULT)FALSE;
}

LRESULT CDialogFrame::OnOK(WORD wNotify, WORD wID, HWND hWnd)
{
    // 编译进行中,忽略重复点击
    if (m_fCompiling)
    {
        return (LRESULT)TRUE;
    }

    std::string exeFile = CStrUtils::TStrToAStr(CPathUtils::GetCurrentModulePath());
    std::string strFileIn = CStrUtils::TStrToAStr(GetItemString(IDC_EDIT_INPUT_FILE));
    std::string strOut = "-out";
    std::string strOutPath = CStrUtils::TStrToAStr(GetItemString(IDC_EDIT_OUTPUT_FILE));
    std::string srcRaw = "-raw";
    std::string srcFns = "-fns";
    std::string srcBank = "-bank";
    std::string srcBankSize = "0";
    std::string srcFill = "-fill";
    std::string srcFillByte = CStrUtils::TStrToAStr(GetItemString(IDC_EDIT_FILL));
    std::string srcSegBasicEnable = "-s";
    std::string srcSegDetailEnable = "-S";
    std::string srcZpOpt = "-zp_opt";

    ::SetCurrentDirectory(CPathUtils::GetFileDir(GetItemString(IDC_EDIT_INPUT_FILE)).c_str());

    std::vector<std::string> args;
    args.push_back(exeFile);
    args.push_back("-in");

    if (!strFileIn.empty())
    {
        args.push_back(strFileIn);
    }

    if (!strOutPath.empty())
    {
        args.push_back(strOut);
        args.push_back(strOutPath);
    }

    if (!srcFillByte.empty())
    {
        args.push_back(srcFill);
        args.push_back(srcFillByte);
    }

    if (IsButtonChecked(IDC_CHECK_SEG))
    {
        if (IsButtonChecked(IDC_CHECK_SEG_DETAIL))
        {
            args.push_back(srcSegDetailEnable);
        }
        else
        {
            args.push_back(srcSegBasicEnable);
        }
    }

    if (IsButtonChecked(IDC_CHECK_RAW))
    {
        args.push_back(srcRaw);
    }

    if (IsButtonChecked(IDC_CHECK_FNS))
    {
        args.push_back(srcFns);
    }

    args.push_back(srcZpOpt);
    if (IsButtonChecked(IDC_CHECK_ZP_OPT))
    {
        args.push_back("1");
    }
    else
    {
        args.push_back("0");
    }

    args.push_back(srcBank);
    if (IsButtonChecked(IDC_RADIO_8K))
    {
        srcBankSize = "0";
    }
    else if (IsButtonChecked(IDC_RADIO_16K))
    {
        srcBankSize = "1";
    }
    else if (IsButtonChecked(IDC_RADIO_32K))
    {
        srcBankSize = "2";
    }
    args.push_back(srcBankSize);

    SetWindowText(g_hEditOutput, _T(""));

    // 清空输出缓冲
    edit_buf_cs_ensure();
    EnterCriticalSection(&g_csEdit);
    memset(edit_buf, 0, sizeof(edit_buf));
    g_edit_len = 0;
    LeaveCriticalSection(&g_csEdit);
    m_shownLen = 0;

    _tstring strMessage;
    m_tmBegin = CTimeUtils::GetCurrentTickCount();
    _tstring strBeginTime = CTimeUtils::GetCurrentTimeString();

    strMessage += LoadString(IDS_STRING_START);
    strMessage += strBeginTime;
    strMessage += _T("\r\n");

    {
        EnterCriticalSection(&g_csEdit);
        strcpy_s(edit_buf, sizeof(edit_buf), CStrUtils::TStrToAStr(strMessage).c_str());
        g_edit_len = (long)strlen(edit_buf);
        LeaveCriticalSection(&g_csEdit);
    }

    // 在后台线程中执行编译,避免阻塞 UI 线程
    StartCompile(args);

    return (LRESULT)TRUE;
}

void CDialogFrame::StartCompile(const std::vector<std::string>& args)
{
    m_fCompiling = true;

    // 禁用编译按钮,防止重复触发
    ::EnableWindow(GetItem(IDOK), FALSE);

    // 启动后台线程执行 main(),main() 中的 fc_printf 会写入
    // edit_buf(由 g_csEdit 保护),UI 线程通过定时器定期刷新显示
    m_compileThread = std::thread([this, args]() {
        std::vector<char*> argv;
        for (size_t i = 0; i < args.size(); i++)
        {
            argv.push_back((char*)args[i].c_str());
        }

        int nResult = main((int)argv.size(), argv.data());

        // 通知 UI 线程编译完成
        ::PostMessage(m_hWnd, WM_APP_COMPILE_DONE, (WPARAM)nResult, 0);
    });

    // 启动定时器,定期把 edit_buf 的内容刷新到输出框
    ::SetTimer(m_hWnd, COMPILE_TIMER_ID, 100, NULL);
}

LRESULT CDialogFrame::OnTimer(WPARAM wParam, LPARAM lParam)
{
    if (wParam == COMPILE_TIMER_ID)
    {
        FlushOutputToEdit();
        return (LRESULT)TRUE;
    }
    return (LRESULT)FALSE;
}

void CDialogFrame::FlushOutputToEdit(void)
{
    if (NULL == g_hEditOutput)
    {
        return;
    }

    // 只拷贝新增部分,避免每次全量 SetWindowText(大输出时开销大)
    EnterCriticalSection(&g_csEdit);
    long len = g_edit_len;
    if (len > m_shownLen)
    {
        std::string strAppend(edit_buf + m_shownLen, len - m_shownLen);
        m_shownLen = len;
        LeaveCriticalSection(&g_csEdit);

        _tstring strText = CStrUtils::AStrToTStr(strAppend);
        if (!strText.empty())
        {
            // 追加到编辑框末尾并滚动到底部
            DWORD dwLen = GetWindowTextLength(g_hEditOutput);
            ::SendMessage(g_hEditOutput, EM_SETSEL, dwLen, dwLen);
            ::SendMessage(g_hEditOutput, EM_REPLACESEL, FALSE, (LPARAM)strText.c_str());
        }
    }
    else
    {
        LeaveCriticalSection(&g_csEdit);
    }
}

LRESULT CDialogFrame::OnCompileDone(WPARAM wParam, LPARAM lParam)
{
    int nResult = (int)wParam;

    // 停止定时器
    ::KillTimer(m_hWnd, COMPILE_TIMER_ID);

    // 恢复编译按钮
    ::EnableWindow(GetItem(IDOK), TRUE);

    m_fCompiling = false;

    // 刷新最后一批输出
    FlushOutputToEdit();

    _tstring strMessage = CStrUtils::AStrToTStr(edit_buf);

    int64_t tmEnd = CTimeUtils::GetCurrentTickCount();
    _tstring strEndTime = CTimeUtils::GetCurrentTimeString();

    strMessage += _T("\r\n");
    strMessage += LoadString(IDS_STRING_END);
    strMessage += strEndTime;

    if (0 == nResult)
    {
        strMessage += _T("\r\n");
        strMessage += LoadString(IDS_STRING_SUCCESS);
    }
    else
    {
        strMessage += _T("\r\n");
        strMessage += LoadString(IDS_STRING_FAILED);
    }
    _tstring strCostTime = LoadString(IDS_STRING_TIME_CONSUMED);
    strCostTime += CStrUtils::Format(_T(": %d ms.\r\n"), (int)(tmEnd - m_tmBegin));
    strMessage += _T("\r\n");
    strMessage += strCostTime;

    ::SetWindowText(g_hEditOutput, strMessage.c_str());
    ::PostMessage(g_hEditOutput, WM_VSCROLL, SB_BOTTOM, 0);

    if (m_fCuiMode)
    {
        FILE* fp;
        if (::AttachConsole(ATTACH_PARENT_PROCESS))
        {
            ::freopen_s(&fp, "CONOUT$", "wt", stdout); // 将stdout重定向到控制台窗口x
            ::_tprintf(strMessage.c_str());
            ::FreeConsole();
        }
        PostMessage(WM_CLOSE, nResult, 0);
    }

    // 编译线程已结束(已 PostMessage),回收线程资源
    if (m_compileThread.joinable())
    {
        m_compileThread.join();
    }

    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnFileInput(WORD wNotify, WORD wID, HWND hWnd)
{
    static _tstring strPath;
    CComFileDialog::CDlgFilterSpec filters = {
    { _T("asm"), _T("*.asm") },
    };

    if (CComFileDialog::GetOpenPath(m_hWnd, strPath, filters))
    {
        SetItemString(IDC_EDIT_INPUT_FILE, strPath);
        m_config.SetString(_T("config"), CONFIG_INPUT_PATH, strPath);
    }

    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnFileOutput(WORD wNotify, WORD wID, HWND hWnd)
{
    static _tstring strPath;
    static UINT uIndex = 0;
    CComFileDialog::CDlgFilterSpec filters = {
    { _T("nes"), _T("*.nes") },
    };


    if (CComFileDialog::GetSavePath(m_hWnd, strPath, filters))
    {
        if (0 != _tcsicmp(CPathUtils::GetFileExt(strPath).c_str(), _T("nes")))
        {
            strPath += _T(".nes");
        }

        SetItemString(IDC_EDIT_OUTPUT_FILE, strPath);
        m_config.SetString(_T("config"), CONFIG_OUTPUT_PATH, strPath);
    }

    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnCancel(WORD wNotify, WORD wID, HWND hWnd)
{
    //EndDialog(IDCANCEL);
    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnCommandExit(WORD wNotify, WORD wID, HWND hWnd)
{
    PostMessage(WM_CLOSE, 0, 0);
    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnSettingsTopmost(WORD wNotify, WORD wID, HWND hWnd)
{
    HMENU hMenu = GetMenu(m_hWnd);

    bool fChecked = !(IsMenuItemChecked(hMenu, wID, false));

    CheckMenuItem(hMenu, wID, fChecked);

    SetWindowPos(m_hWnd, fChecked ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    m_config.SetNumber(_T("config"), CONFIG_TOPMOST_ENABLE, fChecked);

    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnHelpAbout(WORD wNotify, WORD wID, HWND hWnd)
{
    ShellExecute(NULL, _T("open"), HELP_ABOUT_URL, NULL, NULL, SW_SHOW);
    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnLangEnglish(WORD wNotify, WORD wID, HWND hWnd)
{
    HMENU hMenu = GetMenu(m_hWnd);
    CheckMenuItem(hMenu, ID_LANGUAGE_CHINESE, false);
    CheckMenuItem(hMenu, wID, true);

    m_config.SetNumber(_T("config"), CONFIG_LANGUAGE, 1);

    LanguageChange(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));

    return (LRESULT)TRUE;
}

LRESULT CDialogFrame::OnLangChinese(WORD wNotify, WORD wID, HWND hWnd)
{
    HMENU hMenu = GetMenu(m_hWnd);
    CheckMenuItem(hMenu, ID_LANGUAGE_ENGLISH, false);
    CheckMenuItem(hMenu, wID, true);

    m_config.SetNumber(_T("config"), CONFIG_LANGUAGE, 2);

    LanguageChange(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED));

    return (LRESULT)TRUE;
}

void CDialogFrame::LanguageChange(LANGID lang)
{
    SetThreadUILanguage(lang);


    //修改菜单文本
    HMENU hMenu = GetMenu(m_hWnd);
    ModifyMenuText(hMenu, 0, IDS_STRING_FILE, TRUE);

    ModifyMenuText(hMenu, ID_FILE_EXIT, IDS_STRING_FILE_EXIT, FALSE);

    ModifyMenuText(hMenu, 1, IDS_STRING_SETTINGS, TRUE);
    ModifyMenuText(::GetSubMenu(hMenu, 1), 1, IDS_STRING_SETTINGS_LANG, TRUE);
    ModifyMenuText(hMenu, ID_SETTINGS_TOPMOST, IDS_STRING_SETTINGS_TOPMOST, FALSE);

    ModifyMenuText(hMenu, 2, IDS_STRING_HELP, TRUE);
    ModifyMenuText(hMenu, ID_HELP_ABOUT, IDS_STRING_HELP_ABOUT, FALSE);

    //界面文本
    SetItemString(IDC_STATIC_INPUT, LoadString(IDS_STRING_INPUT_FILE));
    SetItemString(IDC_STATIC_OUTPUT, LoadString(IDS_STRING_OUTPUT_FILE));
    SetItemString(IDC_BUTTON_INPUT, LoadString(IDS_STRING_SELECT_FILE));
    SetItemString(IDC_BUTTON_OUTPUT, LoadString(IDS_STRING_SELECT_FILE));
    SetItemString(IDC_CHECK_RAW, LoadString(IDS_STRING_RAW));
    SetItemString(IDC_CHECK_FNS, LoadString(IDS_STRING_FNS));
    SetItemString(IDC_CHECK_SEG, LoadString(IDS_STRING_SEG));
    SetItemString(IDC_CHECK_SEG_DETAIL, LoadString(IDS_STRING_DETAIL));
    SetItemString(IDC_CHECK_ZP_OPT, LoadString(IDS_STRING_ZP_OPT));
    SetItemString(IDC_STATIC_FILL, LoadString(IDS_STRING_FILL));
    SetItemString(IDC_STATIC_BANK, LoadString(IDS_STRING_BANK));
    SetItemString(IDOK, LoadString(IDS_STRING_COMPILE));

    ::DrawMenuBar(m_hWnd);

}
