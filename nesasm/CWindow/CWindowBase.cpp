#include "CWindowBase.h"
#include <strsafe.h>
#include <Winuser.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


BEGIN_WND_BASE_MESSAGE_MAP(CWindowBase)
    ON_WND_MESSAGE(WM_CREATE, &CWindowBase::OnCreate)
    ON_WND_MESSAGE(WM_CLOSE, &CWindowBase::OnClose)
    ON_WND_MESSAGE(WM_ERASEBKGND, &CWindowBase::OnErasebkgnd)
    ON_WND_MESSAGE(WM_DESTROY, &CWindowBase::OnDestroy)
    ON_WND_COMMAND(IDOK, &CWindowBase::OnCommandDemo)
    ON_WND_NOTIFY(LVN_KEYDOWN, IDOK, &CWindowBase::OnNotifyDemo)
END_WND_BASE_MESSAGE_MAP()

CWindowBase::CWindowBase()
    :
    m_hWnd(NULL),
    m_bCenter(FALSE),
    m_SuperProc(nullptr),
    m_uIdSubclass((UINT_PTR)(-1))
{

}

CWindowBase::~CWindowBase()
{
    if (m_hWnd)
    {
        RemoveSubclass();
        ::DestroyWindow(m_hWnd);
    }
}

BOOL CWindowBase::Register(_tstring strClassName)
{
    m_ClassName = strClassName;

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = ::GetModuleHandle(NULL);
    wcex.hIcon = (HICON)::LoadImage(NULL, MAKEINTRESOURCE(OIC_SAMPLE), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
    wcex.hCursor = (HCURSOR)::LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_ClassName.c_str();
    wcex.hIconSm = (HICON)::LoadImage(NULL, MAKEINTRESOURCE(OIC_SAMPLE), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);

    if (::RegisterClassEx(&wcex))
    {
        return TRUE;
    }

    if (ERROR_CLASS_ALREADY_EXISTS == ::GetLastError())
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CWindowBase::Superclass(_tstring strTargetClass, _tstring strNewClassName)
{
    m_ClassName = strNewClassName;

    WNDCLASSEX wcex = { 0 };
    if (!GetClassInfoEx(NULL, strTargetClass.c_str(), &wcex))
    {
        return FALSE;
    }

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    //wcex.lpfnWndProc = WindowProc;
    //wcex.cbClsExtra = 0;
    //wcex.cbWndExtra = 0;
    wcex.hInstance = ::GetModuleHandle(NULL);
    wcex.hIcon = (HICON)::LoadImage(NULL, MAKEINTRESOURCE(OIC_SAMPLE), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
    wcex.hCursor = (HCURSOR)::LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), IMAGE_CURSOR, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
    wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = m_ClassName.c_str();
    wcex.hIconSm = (HICON)::LoadImage(NULL, MAKEINTRESOURCE(OIC_SAMPLE), IMAGE_ICON, 0, 0, LR_SHARED | LR_DEFAULTSIZE);

    m_SuperProc = wcex.lpfnWndProc;
    wcex.lpfnWndProc = WindowProc;
    if (::RegisterClassEx(&wcex))
    {
        return TRUE;
    }

    if (ERROR_CLASS_ALREADY_EXISTS == ::GetLastError())
    {
        return TRUE;
    }

    return FALSE;
}

BOOL CWindowBase::Subclass(HWND hWnd, UINT_PTR uId)
{
    //已经子类化过
    if (m_hWnd && -1 != m_uIdSubclass)
    {
        return FALSE;
    }

    if (::SetWindowSubclass(hWnd, SubclassProc, uId, (DWORD_PTR)this))
    {
        m_hWnd = hWnd;
        m_uIdSubclass = uId;
        return TRUE;
    }

    return FALSE;
}

BOOL CWindowBase::RemoveSubclass()
{
    if (m_hWnd && -1 != m_uIdSubclass)
    {
        RemoveWindowSubclass(m_hWnd, SubclassProc, m_uIdSubclass);
        m_uIdSubclass = (UINT_PTR)(-1);
    }

    return true;
}

BOOL CWindowBase::Create(
    HWND hParent, 
    _tstring strTitle,
    int nCmdShow/* = SW_SHOW*/,
    DWORD dwStyle/* = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN*/,
    DWORD dwExStyle/* = 0*/,
    int dwX/* = 0*/,
    int dwY/* = 0*/,
    int dwWidth/* = 640*/,
    int dwHeight/* = 480*/,
    bool bCenter/* = true*/,
    HMENU hMenu/* = nullptr*/
)
{
    m_bCenter = bCenter;
    RECT rectWnd = { 0, 0, dwWidth, dwHeight };

    ::AdjustWindowRect(&rectWnd, dwStyle, nullptr != hMenu);

    m_hWnd = ::CreateWindowEx(
        dwExStyle, 
        m_ClassName.c_str(), 
        strTitle.c_str(), 
        dwStyle,
        dwX,
        dwY,
        rectWnd.right - rectWnd.left, 
        rectWnd.bottom - rectWnd.top, 
        hParent,
        hMenu,
        NULL, 
        this);

    if (!m_hWnd)
    {
        return FALSE;
    }

    ::ShowWindow(m_hWnd, nCmdShow);
    ::UpdateWindow(m_hWnd);

    return TRUE;
}

UINT_PTR CWindowBase::Run()
{
    MSG msg = { 0 };

    while (::GetMessage(&msg, nullptr, 0, 0))
    {
        if (PreTranslateMessage(&msg))
        {
            continue;
        }

        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }

    return msg.wParam;
}

BOOL CWindowBase::PreTranslateMessage(LPMSG pMsg)
{
    return FALSE;
}

LRESULT CWindowBase::OnCreate(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

LRESULT CWindowBase::OnClose(WPARAM wParam, LPARAM lParam)
{
    RemoveSubclass();
    ::DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
    return FALSE;
}

LRESULT CWindowBase::OnErasebkgnd(WPARAM wParam, LPARAM lParam)
{
    return FALSE;
}

LRESULT CWindowBase::OnDestroy(WPARAM wParam, LPARAM lParam)
{
    if (OnDestory())
    {
        return FALSE;
    }

    //::PostQuitMessage(0);
    return FALSE;
}

LRESULT CWindowBase::OnCommandDemo(WORD wNotify, WORD wID, HWND hWnd)
{
    return FALSE;
}

LRESULT CWindowBase::OnNotifyDemo(WORD wID, LPNMHDR pNmhdr)
{
    return FALSE;
}

LRESULT WINAPI CWindowBase::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWindowBase* pThis = (CWindowBase*)::GetProp(hWnd, _T("this"));
    //CWindowBase* pThis = (CWindowBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);

    if (pThis)
    {
        return pThis->ThisWindowProc(uMsg, wParam, lParam);
    }

    if (WM_NCCREATE == uMsg)
    {
        pThis = (CWindowBase*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        if (pThis)
        {
            pThis->m_hWnd = hWnd;
            //给窗口绑定this指针
            ::SetProp(hWnd, _T("this"), pThis);
            //::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis);
            if (pThis->m_bCenter)
            {
                pThis->MoveToCenter(NULL);
            }

            return pThis->WindowProc(hWnd, uMsg, wParam, lParam);
        }
    }

    return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK CWindowBase::SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{

    CWindowBase* pThis = reinterpret_cast<CWindowBase*>(dwRefData);
    if (pThis)
    {
        return pThis->ThisWindowProc(uMsg, wParam, lParam);
    }

    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CWindowBase::OnCommand(WORD wNotify, WORD wID, HWND hWnd)
{
    return (LRESULT)FALSE;
}

LRESULT CWindowBase::OnNotify(WORD wID, LPNMHDR pNmhdr)
{
    return (LRESULT)FALSE;
}

LRESULT CWindowBase::OnDestory()
{
    return (LRESULT)FALSE;
}

LRESULT CWindowBase::ThisWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const WND_MSGMAP* pEntry = GetMessageMap();
    LRESULT lResult = 0;

    while (pEntry)
    {
        const WND_MSGMAP_ENTRY* lpEntries = pEntry->lpEntries;
        while (lpEntries->m_pFunc.m_pFnMessage)
        {
            if (uMsg == lpEntries->m_nMessage)
            {
                //处理 WM_COMMAND 消息
                if (WM_COMMAND == uMsg)
                {
                    //消息源   HIWORD(wParam)      LOWORD(wParam)     lParam 
                    //菜单     0                   菜单ID             0
                    //快捷键   1                   快捷键ID           0
                    //控件     控件定义的通知码    控件ID             控件窗口句柄
                    WORD wNotify = HIWORD(wParam);
                    WORD wID = LOWORD(wParam);
                    HWND hWnd = (HWND)lParam;

                    lResult = this->OnCommand(wNotify, wID, hWnd);
                    if (lResult)
                    {
                        break;
                    }

                    //ID范围检测
                    if (wID >= lpEntries->m_nID && wID <= lpEntries->m_nID)
                    {
                        lResult = (this->*lpEntries->m_pFunc.m_pFnCommand)(wNotify, wID, hWnd);
                        break;
                    }
                }
                else if (WM_NOTIFY == uMsg)
                {
                    WORD wID = (WORD)wParam;            //发送消息的公共控件的标识符。 不保证此标识符是唯一的。
                    LPNMHDR pNmhdr  = (LPNMHDR)lParam;  //指向包含通知代码和其他信息的 NMHDR 结构的指针

                    lResult = this->OnNotify(wID, pNmhdr);
                    if (lResult)
                    {
                        break;
                    }

                    if (pNmhdr->code == lpEntries->m_nCode && pNmhdr->idFrom >= lpEntries->m_nID && pNmhdr->idFrom <= lpEntries->m_nID)
                    {
                        lResult = (this->*lpEntries->m_pFunc.m_pFnNotify)(wID, pNmhdr);
                        break;
                    }
                }
                else
                {
                    lResult = (this->*lpEntries->m_pFunc.m_pFnMessage)(wParam, lParam);
                    break;
                }
            }

            lpEntries++;
        }

        //消息已经处理
        if (lResult)
        {
            //不希望继续路由
            return lResult;
        }

        //获取基类的消息映射
        pEntry = pEntry->pfnGetBaseMap();
    }

    //超类化处理
    if (m_SuperProc)
    {
        return m_SuperProc(m_hWnd, uMsg, wParam, lParam);
    }

    //默认系统处理
    return ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);
}

bool CWindowBase::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CWindowBase::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

bool CWindowBase::PostCommand(WORD wID, WORD wNotify, LPARAM lParam/* = 0*/)
{
    return ::PostMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(wID, wNotify), lParam);
}

LRESULT CWindowBase::SendCommand(WORD wID, WORD wNotify, LPARAM lParam/* = 0*/)
{
    return ::SendMessage(m_hWnd, WM_COMMAND, MAKEWPARAM(wID, wNotify), lParam);
}

void CWindowBase::ShowWindow(int cmdShow)
{
    ::ShowWindow(m_hWnd, cmdShow);
}

bool CWindowBase::IsVisiable() const
{
    return ::IsWindowVisible(m_hWnd);
}

void CWindowBase::MoveToCenter(HWND hParent)
{
    RECT rectParent = { 0 };
    RECT rectChild = { 0 };
    LONG dwParentW = 0;
    LONG dwParentH = 0;
    LONG dwChildW = 0;
    LONG dwChildH = 0;

    if (NULL == hParent)
    {
        hParent = ::GetDesktopWindow();
    }

    ::GetWindowRect(m_hWnd, &rectChild);
    ::GetWindowRect(hParent, &rectParent);

    dwParentW = rectParent.right - rectParent.left;
    dwParentH = rectParent.bottom - rectParent.top;
    dwChildW = rectChild.right - rectChild.left;
    dwChildH = rectChild.bottom - rectChild.top;

    ::MoveWindow(m_hWnd, rectParent.left + (dwParentW - dwChildW) / 2,
        rectParent.top + (dwParentH - dwChildH) / 2, dwChildW, dwChildH, TRUE);
}

void CWindowBase::ModifyMenuText(HMENU hMnu, UINT uPosition, LPCTSTR lpNewItem, BOOL fByPos)
{
    MENUITEMINFO mii = { 0 };
    TCHAR szBuf[MAX_PATH] = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_DATA;
    mii.fType = MFT_STRING;
    mii.dwTypeData = szBuf;
    mii.cch = _countof(szBuf);

    ::GetMenuItemInfo(hMnu, uPosition, fByPos, &mii);
    ::StringCchCopy(szBuf, _countof(szBuf), lpNewItem);
    ::SetMenuItemInfo(hMnu, uPosition, fByPos, &mii);
}

void CWindowBase::ModifyMenuText(HMENU hMnu, UINT uPosition, UINT resourceID, BOOL fByPos)
{
    ModifyMenuText(hMnu, uPosition, LoadString(resourceID).c_str(), fByPos);
}

_tstring CWindowBase::LoadString(UINT uID)
{
    _tstring strResult;
    LPTSTR lpData = nullptr;

    do
    {
        //获取资源长度
        LPCTSTR lpCStr = nullptr;
        int nLength = ::LoadString(GetModuleHandle(NULL), uID, (LPWSTR)&lpCStr, 0);
        if (0 == nLength)
        {
            break;
        }

        //分配字符串缓冲
        nLength += 1;
        lpData = (LPTSTR)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, nLength * sizeof(TCHAR));
        if (NULL == lpData)
        {
            break;
        }

        //拷贝
        ::StringCchCopy(lpData, nLength, lpCStr);
        strResult = lpData;

    } while (false);

    if (NULL != lpData)
    {
        ::HeapFree(::GetProcessHeap(), 0, lpData);
    }

    return strResult;
}

_tstring CWindowBase::GetItemString(DWORD dwID)
{
    _tstring strResult;
    LPTSTR lpData = nullptr;

    do
    {
        DWORD dwLength = GetWindowTextLength(GetDlgItem(m_hWnd, dwID));
        if (0 == dwLength)
        {
            break;
        }

        dwLength += 1;
        lpData = (LPTSTR)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwLength * sizeof(TCHAR));
        if (NULL == lpData)
        {
            break;
        }

        ::GetDlgItemText(m_hWnd, dwID, lpData, dwLength);
        strResult = lpData;

    } while (false);


    if (NULL != lpData)
    {
        ::HeapFree(::GetProcessHeap(), 0, lpData);
    }

    return strResult;
}

bool CWindowBase::IsButtonChecked(int nIDButton) const
{
    return BST_CHECKED == ::IsDlgButtonChecked(m_hWnd, nIDButton);
}

bool CWindowBase::CheckButton(int nIDButton, bool bCheck/* = true*/)
{
    return ::CheckDlgButton(m_hWnd, nIDButton, bCheck ? BST_CHECKED : BST_UNCHECKED);
}

HWND CWindowBase::GetItem(DWORD dwID) const
{
    return ::GetDlgItem(m_hWnd, dwID);
}

bool CWindowBase::IsMenuItemChecked(HMENU hMenu, UINT uId, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);

    return mii.fState & MF_CHECKED;
}

void CWindowBase::CheckMenuItem(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);

    if (bCheck)
    {
        mii.fState |= MF_CHECKED;
    }
    else
    {

        mii.fState &= ~MF_CHECKED;
    }

    ::SetMenuItemInfo(hMenu, uId, fByPosition, &mii);
}

bool CWindowBase::IsMenuItemEnable(HMENU hMenu, UINT uId, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);

    return (mii.fState & MFS_DISABLED) ? false : true;
}

void CWindowBase::EnableMenuItem(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_STATE;
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);

    if (bCheck)
    {
        mii.fState &= ~MFS_DISABLED;
    }
    else
    {

        mii.fState |= MFS_DISABLED;
    }

    ::SetMenuItemInfo(hMenu, uId, fByPosition, &mii);
}

_tstring CWindowBase::GetMenuItemString(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    TCHAR szBuf[MAX_PATH] = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_DATA;
    mii.dwTypeData = szBuf;
    mii.cch = _countof(szBuf);
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);

    if (nullptr == mii.dwTypeData)
    {
        return _tstring();
    }

    return mii.dwTypeData;
}

bool CWindowBase::SetMenuItemString(HMENU hMenu, UINT uId, const _tstring strText, bool fByPosition/* = false*/)
{
    MENUITEMINFO mii = { 0 };
    TCHAR szBuf[MAX_PATH] = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_DATA;
    mii.dwTypeData = szBuf;
    mii.cch = _countof(szBuf);
    ::GetMenuItemInfo(hMenu, uId, fByPosition, &mii);
    ::StringCchPrintf(szBuf, _countof(szBuf), strText.c_str());
    return ::SetMenuItemInfo(hMenu, uId, fByPosition, &mii);
}

void CWindowBase::SetItemString(DWORD dwID, const _tstring& strText)
{
    ::SetDlgItemText(m_hWnd, dwID, strText.c_str());
}

VOID CWindowBase::ModifyMenuState(HMENU hMnu, UINT uPosition, UINT fState/* = MFS_ENABLED*/, BOOL fByPos/* = TRUE*/)
{
    MENUITEMINFO mii = { 0 };
    TCHAR szBuf[MAX_PATH] = { 0 };
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_TYPE | MIIM_STATE | MIIM_DATA;
    mii.fType = MFT_STRING;
    mii.dwTypeData = szBuf;
    mii.cch = _countof(szBuf);
    ::GetMenuItemInfo(hMnu, uPosition, fByPos, &mii);
    mii.fState = fState;
    ::SetMenuItemInfo(hMnu, uPosition, fByPos, &mii);
}

HWND CWindowBase::GetWndHandle() const
{
    return m_hWnd;
}
