#pragma once

#include <wtypesbase.h>
#include <tchar.h>
#include <string>
#include <vector>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#define HKCR                (HKEY_CLASSES_ROOT)
#define HKCU                (HKEY_CURRENT_USER)
#define HKLM                (HKEY_LOCAL_MACHINE)
#define HKU                 (HKEY_USERS)

#define HKPD                (HKEY_PERFORMANCE_DATA)
#define HKPT                (HKEY_PERFORMANCE_TEXT)
#define HKPN                (HKEY_PERFORMANCE_NLSTEXT)

#if(WINVER >= 0x0400)
#define HKCC                (HKEY_CURRENT_CONFIG)
#define HKDD                (HKEY_DYN_DATA)
#define HKCULS              (HKEY_CURRENT_USER_LOCAL_SETTINGS)
#endif

class CRegUtils
{
public:

    // 写入 REG_DWORD 类型数据
    static bool SetDword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, DWORD dwValue);

    // 读取 REG_DWORD 类型数据
    static bool GetDword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, DWORD& dwValue);
    static DWORD GetDword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, const DWORD& dwDefault);

    // 写入 REG_QWORD 类型数据
    static bool SetQword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, ULONGLONG ullValue);

    // 读取 REG_QWORD 类型数据
    static bool GetQword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, ULONGLONG& ullValue);
    static ULONGLONG GetQword(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName);

    // 写入 REG_SZ 类型数据
    static bool SetString(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, const _tstring& strValue);

    // 读取 REG_SZ 类型数据
    static bool GetString(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, _tstring& strValue);
    static _tstring GetString(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, const _tstring& strDefault = _T(""));

    // 写入 REG_BINARY 类型数据
    static bool SetBinary(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, LPCVOID lpData, DWORD dwSize);

    // 读取 REG_BINARY 类型数据
    static bool GetBinary(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, LPVOID lpData, DWORD cbSize);
    static std::vector<uint8_t> GetBinary(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName);

    // 读取 指定值 所需字节大小
    static bool GetValueSize(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName, DWORD& cbSize);
    static DWORD GetValueSize(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName);

    // 创建 指定键
    static bool CreateKey(HKEY hKey, const _tstring& lpSubKey);

    // 删除 指定键(递归删除)
    static bool DelKey(HKEY hKey, const _tstring& lpSubKey);

    // 删除 指定值
    static bool DelValue(HKEY hKey, const _tstring& lpSubKey, const _tstring& lpValueName);

    // 枚举 指定键 的子键
    static bool EnumKeyName(HKEY hKey, const _tstring& lpSubKey, std::vector<_tstring>& keyList);
    static std::vector<_tstring> EnumKeyName(HKEY hKey, const _tstring& lpSubKey);

    // 枚举 指定键 的值名
    static bool EnumValueName(HKEY hKey, const _tstring& lpSubKey, std::vector<_tstring>& valueList);
    static std::vector<_tstring> EnumValueName(HKEY hKey, const _tstring& lpSubKey);
};
