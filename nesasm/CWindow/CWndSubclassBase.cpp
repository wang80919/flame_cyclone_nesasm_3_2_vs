#include "CWndSubclassBase.h"
#include <strsafe.h>
#include <windowsx.h>
#include <Winuser.h>

#include "commctrl.h"
#pragma comment(lib, "Comctl32.lib")

BEGIN_SUBCLASS_BASE_MESSAGE_MAP(CWndSubclassBase)
    ON_SUBCLASS_MESSAGE(WM_NCHITTEST, &CWndSubclassBase::OnNcHitTest)
END_SUBCLASS_BASE_MESSAGE_MAP()

CWndSubclassBase::CWndSubclassBase()
    :
    m_hWnd(NULL),
    m_uId(0)
{

}

CWndSubclassBase::~CWndSubclassBase()
{
    RemoveSubclass();
}

bool CWndSubclassBase::SetSubclass(HWND hWnd, UINT_PTR uId)
{
    if (NULL == hWnd)
    {
        return false;
    }

    if (m_hWnd)
    {
        RemoveSubclass();
    }

    if (::SetWindowSubclass(hWnd, SubClassProc, uId, (DWORD_PTR)this))
    {
        m_hWnd = hWnd;
        m_uId = uId;
        return true;
    }

    return false;
}

bool CWndSubclassBase::RemoveSubclass()
{
    if (m_hWnd)
    {
        return ::RemoveWindowSubclass(m_hWnd, SubClassProc, m_uId);
    }

    return true;
}

LRESULT CALLBACK CWndSubclassBase::SubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    CWndSubclassBase* pThis = reinterpret_cast<CWndSubclassBase*>(GetProp(hWnd, _T("this")));
    if (pThis)
    {
        return pThis->ThisWindowProc(uMsg, wParam, lParam);
    }

    return ::DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CWndSubclassBase::ThisWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    const WND_SUBCLASS_MSGMAP* pEntry = GetMessageMap();
    LRESULT lResult = 0;

    while (pEntry)
    {
        const WND_SUBCLASS_MSGMAP_ENTRY* lpEntries = pEntry->lpEntries;
        while (lpEntries->m_pfn)
        {
            if (uMsg == lpEntries->m_nMessage)
            {
                //处理 WM_COMMAND 消息
                if (WM_COMMAND == uMsg)
                {
                    if (wParam == lpEntries->m_nID)
                    {
                        lResult = (this->*lpEntries->m_pfn)(wParam, lParam);
                        break;
                    }
                }
                else
                {
                    lResult = (this->*lpEntries->m_pfn)(wParam, lParam);
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

    return ::DefSubclassProc(m_hWnd, uMsg, wParam, lParam);
}

VOID CWndSubclassBase::ShowWindow(int cmdShow)
{
    ::ShowWindow(m_hWnd, cmdShow);
}

BOOL CWndSubclassBase::IsVisiable() const
{
    return ::IsWindowVisible(m_hWnd);
}

void CWndSubclassBase::MoveToCenter(HWND hParent)
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

_tstring CWndSubclassBase::LoadString(UINT uID)
{
    static TCHAR szBuf[1024] = { 0 };
    ::LoadString(GetModuleHandle(NULL), uID, szBuf, _countof(szBuf));
    return szBuf;
}

VOID CWndSubclassBase::ModifyMenuText(HMENU hMnu, UINT uPosition, LPCTSTR lpNewItem, BOOL fByPos)
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

VOID CWndSubclassBase::ModifyMenuText(HMENU hMnu, UINT uPosition, UINT resourceID, BOOL fByPos)
{
    ModifyMenuText(hMnu, uPosition, LoadString(resourceID).c_str(), fByPos);
}

HWND CWndSubclassBase::GetWndHandle() const
{
    return m_hWnd;
}

BOOL CWndSubclassBase::PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::PostMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CWndSubclassBase::SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return ::SendMessage(m_hWnd, uMsg, wParam, lParam);
}

LRESULT CWndSubclassBase::OnNcHitTest(WPARAM wParam, LPARAM lParam)
{
    return ::DefSubclassProc(m_hWnd, WM_NCHITTEST, wParam, lParam);
}