#pragma once

#include <wtypesbase.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif


class CCommandLine
{
public:

	// 获取命令行
	static _tstring GetCommand();

    //
    // @brief: 获取命令行键值对
    // @param: chDelim                      键值对分割符号
    // @ret: std::map<_tstring, _tstring>   命令行键值对
	static std::unordered_map<_tstring, _tstring> GetCommandKeyValue(TCHAR chDelim = _T('\0'));
    static std::vector<_tstring> GetCommandList();

private:

    // 宽字符串转换
    static _tstring _WStrToTStr(const std::wstring& str);

    static std::wstring _TStrToWStr(const _tstring& str);

    // 宽字符转其他
    static std::string _WStrToMultiStr(UINT CodePage, const std::wstring& str);

    // 其他转宽字符
    static std::wstring _MultiStrToWStr(UINT CodePage, const std::string& str);
};

