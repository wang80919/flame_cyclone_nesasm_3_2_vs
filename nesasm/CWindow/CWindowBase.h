#pragma once

#define OEMRESOURCE

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <commctrl.h>
#include <vector>

#pragma warning(disable:4100)

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CWindowBase;

typedef LRESULT(CWindowBase::* pMessageFn)(WPARAM wParam, LPARAM lParam);
typedef LRESULT(CWindowBase::* pCommandFn)(WORD wNotify, WORD wID, HWND hWnd);
typedef LRESULT(CWindowBase::* pNotifyFn)(WORD wID, LPNMHDR pNmhdr);

struct WND_MSGMAP_ENTRY
{
    union
    {
        pMessageFn m_pFnMessage;
        pCommandFn m_pFnCommand;
        pNotifyFn m_pFnNotify;

    }m_pFunc;               //要调用的例程（或特殊值）
    UINT m_nMessage;        //Windows消息
    UINT m_nCode;           //控件码或 or WM_NOTIFY code
    UINT m_nID;             //控件ID (Windows消息则为0)
    UINT m_nLastID;         //用于指定控件 ID 范围的条目

    WND_MSGMAP_ENTRY()
        :
        m_nMessage(0),
        m_nCode(0),
        m_nID(0),
        m_nLastID(0)
    {
        m_pFunc.m_pFnMessage = nullptr;
    }

    WND_MSGMAP_ENTRY(UINT uMsg, pMessageFn pfn)
        : 
        m_nMessage(uMsg), 
        m_nCode(0),
        m_nID(0),
        m_nLastID(0)
    {
        m_pFunc.m_pFnMessage = pfn;
    }

    WND_MSGMAP_ENTRY(UINT uMsg, UINT id, pCommandFn pfn)
        :
        m_nMessage(uMsg),
        m_nCode(0),
        m_nID(id),
        m_nLastID(0)
    {
        m_pFunc.m_pFnCommand = pfn;
    }

    WND_MSGMAP_ENTRY(UINT uMsg, UINT id, UINT idLast, pCommandFn pfn)
        :
        m_nMessage(uMsg),
        m_nCode(0),
        m_nID(id),
        m_nLastID(idLast)
    {
        m_pFunc.m_pFnCommand = pfn;
    }

    WND_MSGMAP_ENTRY(UINT uMsg, UINT wNotifyCode, UINT id, UINT idLast, pNotifyFn pfn)
        :
        m_nMessage(uMsg),
        m_nCode(wNotifyCode),
        m_nID(id),
        m_nLastID(idLast)
    {
        m_pFunc.m_pFnNotify = pfn;
    }
};

struct WND_MSGMAP
{
    const WND_MSGMAP* (*pfnGetBaseMap)();
    const WND_MSGMAP_ENTRY* lpEntries;
};

#ifndef wnd_msg
#define wnd_msg
#endif

#ifndef wnd_command
#define wnd_command
#endif

#ifndef wnd_notify
#define wnd_notify
#endif

#define DECLARE_WND_MESSAGE_MAP()                               \
protected:                                                      \
static const WND_MSGMAP* GetThisMessageMap();                   \
virtual const WND_MSGMAP* GetMessageMap() const;                \

#define BEGIN_WND_MESSAGE_MAP(theClass, baseClass)              \
const WND_MSGMAP* theClass::GetMessageMap() const               \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_MSGMAP* theClass::GetThisMessageMap()                 \
{                                                               \
    typedef baseClass TheBaseClass;					            \
static const WND_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \
 
#define ON_WND_MESSAGE(_msg, _Fn)                               \
WND_MSGMAP_ENTRY(_msg, static_cast< LRESULT (CWindowBase::*)(WPARAM, LPARAM) >(_Fn)),

#define ON_WND_COMMAND(_id, _Fn)                                \
WND_MSGMAP_ENTRY(WM_COMMAND, _id, static_cast< LRESULT (CWindowBase::*)(WORD wNotify, WORD wID, HWND hWnd) >(_Fn)),

#define ON_WND_COMMAND_RANGE(_id, _idLast, _Fn)                 \
WND_MSGMAP_ENTRY(WM_COMMAND, _id, _idLast, static_cast< LRESULT (CWindowBase::*)(WORD wNotify, WORD wID, HWND hWnd) >(_Fn)),

#define ON_WND_NOTIFY(_wNotifycode, _id, _Fn)                   \
WND_MSGMAP_ENTRY(WM_NOTIFY, (UINT)_wNotifycode, _id, _id,  static_cast< LRESULT (CWindowBase::*)(WORD wID, LPNMHDR pNmhdr) >(_Fn)),

#define ON_WND_NOTIFY_RANGE(_wNotifycode, _id, _idLast, _Fn)    \
WND_MSGMAP_ENTRY(WM_NOTIFY, (UINT)_wNotifycode, _id, _idLast,  static_cast< LRESULT (CWindowBase::*)(WORD wID, LPNMHDR pNmhdr) >(_Fn)),

#define END_WND_MESSAGE_MAP()  WND_MSGMAP_ENTRY(0, nullptr) };  \
static const WND_MSGMAP messageMap =                            \
{&TheBaseClass::GetThisMessageMap , &_messageEntries[0] };      \
    return &messageMap;                                         \
}                                                               \

#define DECLARE_WND_BASE_MESSAGE_MAP()                          \
protected:                                                      \
static const WND_MSGMAP* GetThisMessageMap();                   \
virtual const WND_MSGMAP* GetMessageMap() const;                \
static const WND_MSGMAP* GetBaseMessageMap();                   \

#define BEGIN_WND_BASE_MESSAGE_MAP(theClass)                    \
typedef theClass TheThisClass;					                \
const WND_MSGMAP* TheThisClass::GetMessageMap() const           \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_MSGMAP* TheThisClass::GetBaseMessageMap()             \
{                                                               \
    return NULL;                                                \
}                                                               \
const WND_MSGMAP* TheThisClass::GetThisMessageMap()             \
{                                                               \
static const WND_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \

#define END_WND_BASE_MESSAGE_MAP()  WND_MSGMAP_ENTRY(0, nullptr) };    \
static const WND_MSGMAP messageMap =                            \
{&TheThisClass::GetBaseMessageMap, &_messageEntries[0] };       \
return &messageMap;                                             \
}                                                               \

typedef struct _SUBCLASS_WND_INFO
{
    HWND hWnd = NULL;
    UINT_PTR uID = (UINT_PTR)(-1);

    _SUBCLASS_WND_INFO(HWND hWnd, UINT_PTR uID)
    {
        this->hWnd = hWnd;
        this->uID = uID;
    }

}SUBCLASS_WND_INFO;

class CWindowBase
{
public:

    CWindowBase();
    CWindowBase(const CWindowBase& r) = delete;
    CWindowBase& operator = (const CWindowBase& r) = delete;
    ~CWindowBase();

    //注册窗口类
    virtual BOOL Register(_tstring strClassName);

    //超类化窗口
    virtual BOOL Superclass(_tstring strTargetClass, _tstring strClassName);

    //子类化窗口
    virtual BOOL Subclass(HWND hWnd, UINT_PTR uId);
    BOOL RemoveSubclass();

    //创建窗口
    virtual BOOL Create(
        HWND hParent,
        _tstring strTitle,
        int nCmdShow = SW_SHOW,
        DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        DWORD dwExStyle = 0,
        int dwX = 0,
        int dwY = 0,
        int dwWidth = 640,
        int dwHeight = 480,
        bool bCenter = true,
        HMENU hMenu = nullptr
    );

    //命令消息处理
    virtual LRESULT OnCommand(WORD wNotify, WORD wID, HWND hWnd);

    //通知消息处理
    virtual LRESULT OnNotify(WORD wID, LPNMHDR pNmhdr);

    //销毁处理
    virtual LRESULT OnDestory();

    //执行消息环
    UINT_PTR Run();

public:

    //消息
    bool PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam = 0);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam = 0);
    bool PostCommand(WORD wID, WORD wNotify = 0, LPARAM lParam = 0);
    LRESULT SendCommand(WORD wID, WORD wNotify = 0, LPARAM lParam = 0);

    //显示窗口
    void ShowWindow(int cmdShow = SW_SHOW);

    //检查窗口是否显示
    bool IsVisiable() const;

    //移动窗口到父窗口中心
    void MoveToCenter(HWND hParent);

    //修改菜单文本
    void ModifyMenuText(HMENU hMnu, UINT uPosition, LPCTSTR lpNewItem, BOOL fByPos = TRUE);

    //修改菜单文本为资源字符串
    void ModifyMenuText(HMENU hMnu, UINT uPosition, UINT resourceID, BOOL fByPos = TRUE);

    //修改菜单状态
    void ModifyMenuState(HMENU hMnu, UINT uPosition, UINT fState = MFS_ENABLED, BOOL fByPos = TRUE);

    //获取窗口句柄
    HWND GetWndHandle() const;

    //加载资源字符串
    _tstring LoadString(UINT resourceID);

    //获取子项文本
    _tstring GetItemString(DWORD dwID);

    //设置子项文本
    void SetItemString(DWORD dwID, const _tstring& strText);

    //检查子项按钮是否勾选
    bool IsButtonChecked(int nIDButton) const;

    //勾选子项按钮
    bool CheckButton(int nIDButton, bool bCheck = true);

    //获取子项窗口句柄
    HWND GetItem(DWORD dwID) const;

    //检查菜单项是否勾选
    bool IsMenuItemChecked(HMENU hMenu, UINT uId, bool fByPosition = false);

    //勾选菜单项
    void CheckMenuItem(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition = false);

    //检查菜单项是否启用
    bool IsMenuItemEnable(HMENU hMenu, UINT uId, bool fByPosition = false);

    //启用菜单项
    void EnableMenuItem(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition = false);

    //获取菜单项文本
    _tstring GetMenuItemString(HMENU hMenu, UINT uId, bool bCheck, bool fByPosition = false);

    //设置菜单项文本
    bool SetMenuItemString(HMENU hMenu, UINT uId, const _tstring strText, bool fByPosition = false);

    operator HWND() const
    {
        return m_hWnd;
    }

protected:

    wnd_msg LRESULT OnCreate(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnClose(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnErasebkgnd(WPARAM wParam, LPARAM lParam);
    wnd_msg LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);

    wnd_command LRESULT OnCommandDemo(WORD wNotify, WORD wID, HWND hWnd);
    wnd_notify LRESULT OnNotifyDemo(WORD wID, LPNMHDR pNmhdr);

private:

    static LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    LRESULT ThisWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:

    virtual BOOL PreTranslateMessage(LPMSG pMsg);

protected:
    _tstring    m_ClassName;
    HWND        m_hWnd;
    BOOL        m_bCenter;
    WNDPROC     m_SuperProc = nullptr;
    UINT_PTR    m_uIdSubclass = (UINT_PTR)(-1);

    DECLARE_WND_BASE_MESSAGE_MAP()
};