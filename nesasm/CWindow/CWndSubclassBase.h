#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <commctrl.h>

#pragma warning(disable:4100)

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

class CWndSubclassBase;

typedef LRESULT(CWndSubclassBase::* pMapFn)(WPARAM wParam, LPARAM lParam);

struct WND_SUBCLASS_MSGMAP_ENTRY
{
    pMapFn m_pfn;
    UINT m_nMessage;
    UINT m_nID;
    WND_SUBCLASS_MSGMAP_ENTRY(UINT uMsg, pMapFn pfn) : m_nMessage(uMsg), m_pfn(pfn), m_nID(0) {}
    WND_SUBCLASS_MSGMAP_ENTRY(UINT uMsg, UINT nID, pMapFn pfn) : m_nMessage(uMsg), m_nID(nID), m_pfn(pfn) {}
};

struct WND_SUBCLASS_MSGMAP
{
    const WND_SUBCLASS_MSGMAP* (*pfnGetBaseMap)();
    const WND_SUBCLASS_MSGMAP_ENTRY* lpEntries;
};

#ifndef wnd_msg
#define wnd_msg
#endif

#define DECLARE_SUBCLASS_MESSAGE_MAP()                                   \
protected:                                                      \
static const WND_SUBCLASS_MSGMAP* GetThisMessageMap();                   \
virtual const WND_SUBCLASS_MSGMAP* GetMessageMap() const;                \

#define BEGIN_SUBCLASS_MESSAGE_MAP(theClass, baseClass)                  \
const WND_SUBCLASS_MSGMAP* theClass::GetMessageMap() const               \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_SUBCLASS_MSGMAP* theClass::GetThisMessageMap()                 \
{                                                               \
    typedef baseClass TheBaseClass;					            \
static const WND_SUBCLASS_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \

#define ON_SUBCLASS_MESSAGE(message, memberFxn)                          \
WND_SUBCLASS_MSGMAP_ENTRY(message, static_cast< LRESULT (CWndSubclassBase::*)(WPARAM, LPARAM) >(memberFxn)),


#define ON_SUBCLASS_COMMAND(_id, memberFxn)                          \
WND_SUBCLASS_MSGMAP_ENTRY(WM_COMMAND, _id, static_cast< LRESULT (CWndSubclassBase::*)(WPARAM, LPARAM) >(memberFxn)),

#define END_SUBCLASS_MESSAGE_MAP()  WND_SUBCLASS_MSGMAP_ENTRY(0, NULL) };         \
static const WND_SUBCLASS_MSGMAP messageMap =                            \
{&TheBaseClass::GetThisMessageMap , &_messageEntries[0] };      \
return &messageMap;                                             \
}                                                               \

#define DECLARE_SUBCLASS_BASE_MESSAGE_MAP()                              \
protected:                                                      \
static const WND_SUBCLASS_MSGMAP* GetThisMessageMap();                   \
virtual const WND_SUBCLASS_MSGMAP* GetMessageMap() const;                \
static const WND_SUBCLASS_MSGMAP* GetBaseMessageMap();                   \

#define BEGIN_SUBCLASS_BASE_MESSAGE_MAP(theClass)                        \
typedef theClass TheThisClass;					                \
const WND_SUBCLASS_MSGMAP* TheThisClass::GetMessageMap() const           \
{                                                               \
    return GetThisMessageMap();                                 \
}                                                               \
const WND_SUBCLASS_MSGMAP* TheThisClass::GetBaseMessageMap()             \
{                                                               \
    return NULL;                                                \
}                                                               \
const WND_SUBCLASS_MSGMAP* TheThisClass::GetThisMessageMap()             \
{                                                               \
static const WND_SUBCLASS_MSGMAP_ENTRY _messageEntries[] =               \
{                                                               \

#define END_SUBCLASS_BASE_MESSAGE_MAP()  WND_SUBCLASS_MSGMAP_ENTRY(0, NULL) };    \
static const WND_SUBCLASS_MSGMAP messageMap =                            \
{&TheThisClass::GetBaseMessageMap, &_messageEntries[0] };       \
return &messageMap;                                             \
}                                                               \

class CWndSubclassBase
{
public:

    CWndSubclassBase();
    CWndSubclassBase(const CWndSubclassBase& r) = delete;
    ~CWndSubclassBase();

    bool SetSubclass(HWND hWnd, UINT_PTR uId);
    bool RemoveSubclass();

public:
    VOID ShowWindow(int cmdShow = SW_SHOW);
    BOOL IsVisiable() const;
    VOID MoveToCenter(HWND hParent);
    _tstring LoadString(UINT resourceID);
    VOID ModifyMenuText(HMENU hMnu, UINT uPosition, LPCTSTR lpNewItem, BOOL fByPos = TRUE);
    VOID ModifyMenuText(HMENU hMnu, UINT uPosition, UINT resourceID, BOOL fByPos = TRUE);
    HWND GetWndHandle() const;
    BOOL PostMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT SendMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    operator HWND() const
    {
        return m_hWnd;
    }

protected:

    wnd_msg LRESULT OnNcHitTest(WPARAM wParam, LPARAM lParam);

private:

    static LRESULT CALLBACK SubClassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    LRESULT ThisWindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    HWND         m_hWnd;
    UINT_PTR     m_uId;

    DECLARE_SUBCLASS_BASE_MESSAGE_MAP()
};