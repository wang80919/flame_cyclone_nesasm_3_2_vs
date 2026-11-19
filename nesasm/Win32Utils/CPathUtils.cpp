#include "CPathUtils.h"
#include <windows.h>
#include <shlwapi.h>
#include <io.h>

#pragma comment(lib, "Shlwapi.lib")

_tstring CPathUtils::GetCurrentModulePath()
{
    TCHAR szCurPath[MAX_PATH] = { 0 };
    ::GetModuleFileName(NULL, szCurPath, _countof(szCurPath));

    return szCurPath;
}

_tstring CPathUtils::GetCurrentModuleDir()
{
    return GetFileDir(GetCurrentModulePath());
}

_tstring CPathUtils::GetCurrentModuleName(bool bHasExt/* = true*/)
{
    return GetFileName(GetCurrentModulePath(), bHasExt);
}

_tstring CPathUtils::GetFileDir(const _tstring& strPath)
{
    _tstring strResult;
    size_t nIndex = strPath.find_last_of(_T('\\'));
    if (nIndex != _tstring::npos)
    {
        strResult = strPath.substr(0, nIndex);
    }

    return strResult;
}

_tstring CPathUtils::GetFileName(const _tstring& strPath, bool bHasExt/* = true*/)
{
    _tstring strResult = strPath;
    size_t nIndex = strResult.find_last_of(_T('\\'));
    if (nIndex != _tstring::npos)
    {
        strResult = strResult.substr(nIndex + 1);
    }

    if (!bHasExt)
    {
        nIndex = strResult.find_last_of(_T('.'));
        if (nIndex != _tstring::npos)
        {
            strResult.resize(nIndex);
        }
    }

    return strResult;
}

_tstring CPathUtils::GetFileExt(const _tstring& strPath, bool bHasDot/* = true*/)
{
    _tstring strResult = strPath;
    size_t nIndex = strResult.find_last_of(_T('.'));
    if (nIndex != _tstring::npos)
    {
        if (bHasDot)
        {
            strResult = strResult.substr(nIndex);
        }
        else
        {
            strResult = strResult.substr(nIndex + 1);
        }
    }
    else
    {
        strResult = _T("");
    }

    return strResult;
}

_tstring CPathUtils::GetShortPath(const _tstring& strPath)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::GetShortPathName(strPath.c_str(), szBuf, _countof(szBuf));
    return szBuf;
}

_tstring CPathUtils::GetLongPath(const _tstring& strPath)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::GetLongPathName(strPath.c_str(), szBuf, _countof(szBuf));
    return szBuf;
}

_tstring CPathUtils::GetFullPath(const _tstring& strPath)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    TCHAR* pFilePart = nullptr;
    ::GetFullPathName(strPath.c_str(), _countof(szBuf), szBuf, &pFilePart);
    return szBuf;
}

_tstring CPathUtils::DosPathToNtPath(const _tstring& strPath)
{
    _tstring strResultPath;
    TCHAR szDriveStrings[MAX_PATH] = { 0 };
    TCHAR szDosBuf[MAX_PATH] = { 0 };
    TCHAR szResultBuf[MAX_PATH] = { 0 };
    LPTSTR pDriveStr = NULL;

    // 获取盘符名到缓冲
    if (::GetLogicalDriveStrings(_countof(szDriveStrings), szDriveStrings))
    {
        // 遍历盘符名
        for (int i = 0; i < _countof(szDriveStrings); i += 4)
        {
            pDriveStr = &szDriveStrings[i];
            pDriveStr[2] = _T('\0');

            // 查询盘符对应的DOS设备名称
            if (!::QueryDosDevice(pDriveStr, szDosBuf, _countof(szDosBuf)))
            {
                break;
            }

            // 对比路径前缀
            size_t nLen = _tcslen(szDosBuf);
            if (0 == _tcsnicmp(strPath.c_str(), szDosBuf, nLen))
            {
                lstrcpy(szResultBuf, pDriveStr);
                lstrcat(szResultBuf, strPath.c_str() + nLen);
                strResultPath = szResultBuf;
                break;
            }
        }
    }

    return strResultPath;
}

_tstring CPathUtils::NtPathToDosPath(const _tstring& strPath)
{
    _tstring strResultPath;
    TCHAR szDriveStrings[MAX_PATH] = { 0 };
    TCHAR szDosBuf[MAX_PATH] = { 0 };
    TCHAR szResultBuf[MAX_PATH] = { 0 };
    LPTSTR pDriveStr = NULL;

    // 获取盘符名到缓冲
    if (::GetLogicalDriveStrings(_countof(szDriveStrings), szDriveStrings))
    {
        // 遍历盘符名
        for (int i = 0; i < _countof(szDriveStrings); i += 4)
        {
            pDriveStr = &szDriveStrings[i];
            pDriveStr[2] = _T('\0');

            // 查询盘符对应的DOS设备名称
            if (!::QueryDosDevice(pDriveStr, szDosBuf, _countof(szDosBuf)))
            {
                break;
            }

            // 对比路径前缀
            size_t nLen = _tcslen(pDriveStr);
            if (0 == _tcsnicmp(strPath.c_str(), pDriveStr, nLen))
            {
                lstrcpy(szResultBuf, szDosBuf);
                lstrcat(szResultBuf, strPath.c_str() + nLen);
                strResultPath = szResultBuf;
                break;
            }
        }
    }

    return strResultPath;
}

bool CPathUtils::GetAttributes(const _tstring& strPath, DWORD& dwAttr)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    dwAttr = attr.dwFileAttributes;
    return true;
}

DWORD CPathUtils::GetAttributes(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return 0;
    }

    return attr.dwFileAttributes;
}

bool CPathUtils::SetAttributes(const _tstring& strPath, const DWORD& dwAttr)
{
    return ::SetFileAttributes(strPath.c_str(), dwAttr);
}

unsigned long long CPathUtils::GetFileSize(const _tstring& strPath)
{
    unsigned long long ullSize = 0;
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return 0;
    }
    
    if (attr.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return 0;
    }

    ullSize = (unsigned long long)attr.nFileSizeHigh << 32 | attr.nFileSizeLow;

    return ullSize;
}

bool CPathUtils::IsFile(const _tstring& strPath)
{
    DWORD dwAttr = 0;
    if (!GetAttributes(strPath, dwAttr))
    {
        return false;
    }

    return 0 == (dwAttr & FILE_ATTRIBUTE_DIRECTORY);
}

bool CPathUtils::IsDirectory(const _tstring& strPath)
{
    return 0 != (GetAttributes(strPath) & FILE_ATTRIBUTE_DIRECTORY);
}

bool CPathUtils::IsExist(const _tstring& strPath)
{
    return 0 == _taccess_s(strPath.c_str(), 00);
}

bool CPathUtils::IsReadonly(const _tstring& strPath)
{
    return 0 != (GetAttributes(strPath) & FILE_ATTRIBUTE_READONLY);
}

bool CPathUtils::IsHidden(const _tstring& strPath)
{
    WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
    if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
    {
        return false;
    }

    return attr.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN;
}

bool CPathUtils::IsFullPath(const _tstring& strPath)
{
    if (::PathIsRelative(strPath.c_str()) || ::PathIsUNC(strPath.c_str()))
    {
        return false;
    }

    return true;
}

_tstring CPathUtils::GetRelativePath(const _tstring& strDest, const _tstring& strSrc, bool bFromIsDir/* = true*/)
{
    _TCHAR szRelativePath[MAX_PATH] = { 0 };
    DWORD dwAttrFrom = 0;

    if (bFromIsDir)
    {
        dwAttrFrom = FILE_ATTRIBUTE_DIRECTORY;
    }

    if (::PathRelativePathTo(szRelativePath, strSrc.c_str(), dwAttrFrom, strDest.c_str(), 0))
    {
        return szRelativePath;
    }

    return strDest;
}

bool CPathUtils::IsRelative(const _tstring& strPath)
{
    return ::PathIsRelative(strPath.c_str());
}

bool CPathUtils::IsAbsolutePath(const _tstring& strPath)
{
    return !::PathIsRelative(strPath.c_str());
}

bool CPathUtils::QuoteSpaces(_tstring& strPath)
{
    if (strPath.empty())
    {
        return false;
    }

    size_t nFindBegin = strPath.find_first_of(_T('\"'));
    size_t nFindEnd = strPath.find_last_of(_T('\"'));

    //两边已经有引号
    if (0 == nFindBegin && (strPath.size() - 1 == nFindEnd))
    {
        return false;
    }

    //检查是否有空格
    if (_tstring::npos == strPath.find(_T(' ')))
    {
        return false;
    }

    strPath = _T("\"") + strPath + _T("\"");
    return true;
}

bool CPathUtils::UnquoteSpaces(_tstring& strPath)
{
    size_t nFindBegin = strPath.find_first_of(_T('\"'));
    size_t nFindEnd = strPath.find_last_of(_T('\"'));

    if (_tstring::npos == nFindBegin || _tstring::npos == nFindEnd)
    {
        return false;
    }

    strPath = strPath.substr(nFindBegin + 1, strPath.size() - 2);

    return true;
}

_tstring CPathUtils::GetAbsolutePath(const _tstring& strPath)
{
    _tstring strResult = strPath;
    if (PathIsRelative(strPath.c_str()))
    {
        if (strPath.empty())
        {
            strResult = GetCurrentModuleDir();
        }
        else
        {
            strResult = GetCurrentModuleDir() + _T("\\") + strPath;
        }
    }

    return strResult;
}

bool CPathUtils::Rename(const _tstring& strSrc, const _tstring& strDest)
{
    return Move(strSrc, strDest);
}

bool CPathUtils::Move(const _tstring& strSrc, const _tstring& strDest)
{
    _tstring strSrcPath = strSrc;
    _tstring strDestPath = strDest;

    //不存在则提前返回
    if (!IsExist(strSrcPath))
    {
        return false;
    }

    strSrcPath = GetAbsolutePath(strSrc);
    strDestPath = GetAbsolutePath(strDest);

    _tstring strSrcName = GetFileName(strSrc, true);
    _tstring strDestDir = GetFileDir(strDestPath);

    //末尾为反斜杠则认为是移动到文件夹内
    if (_T('\\') == strDestPath.back())
    {
        strDestPath += strSrcName;
    }

    //检查目标文件夹, 不存在则创建
    if (!IsDirectory(strDestDir))
    {
        CreateDir(strDestDir);
    }

    return ::MoveFileEx(strSrcPath.c_str(), strDestPath.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}

bool CPathUtils::Copy(
    const _tstring& strSrc, 
    const _tstring& strDest, 
    bool bEmptyDir/* = true*/, 
    bool bOverwrites/* = false*/,
    int32_t depth/* = MAX_PATH*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return true;
    }

    if (strSrc.empty() || strDest.empty())
    {
        return false;
    }

    _tstring strSrcName = GetFileName(strSrc, true);

    // 文件拷贝
    if (IsFile(strSrc))
    {
        // 拷贝到目录
        if (_T('\\') == strDest.back())
        {
            _tstring strDir = strDest.substr(0, strDest.size() - 1);
            _tstring strFile = strDest + strSrcName;
            if (!strDir.empty() && !IsDirectory(strDir))
            {
                CreateDir(strDir);
            }

            if (!bOverwrites && IsFile(strFile))
            {
                return true;
            }

            return ::CopyFile(strSrc.c_str(), strFile.c_str(), FALSE);
        }
        else
        {
            _tstring strDestDir = strDest;
            _tstring strDestName = strSrcName;

            if (!IsDirectory(strDest))
            {
                strDestDir = GetFileDir(strDest);
                strDestName = GetFileName(strDest, true);
            }

            if (!strDestDir.empty() && !IsDirectory(strDestDir))
            {
                CreateDir(strDestDir);
            }

            _tstring strDestFile = strDestName;
            if (!strDestDir.empty())
            {
                strDestFile = strDestDir + _T("\\") + strDestName;
            }

            if (!bOverwrites && IsFile(strDestFile))
            {
                return true;
            }

            return ::CopyFile(strSrc.c_str(), strDestFile.c_str(), FALSE);
        }
    }
    else
    {
        // 不是文件夹且所在文件夹不存在则认为失败
        if (!IsDirectory(strSrc) && !IsDirectory(GetFileDir(strSrc)))
        {
            return false;
        }

        _tstring strCopyDest = strDest;
        if (IsDirectory(strSrc) && _T('\\') == strCopyDest.back())
        {
            strCopyDest = strDest + strSrcName;
        }

        return CopyDir(strSrc, strCopyDest, bEmptyDir, bOverwrites, depth);
    }

    return false;
}

bool CPathUtils::CopyDir(
    const _tstring& strSrc, 
    const _tstring& strDest, 
    bool bEmptyDir/* = true*/, 
    bool bOverwrites/* = false*/,
    int32_t depth/* = MAX_PATH*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return true;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;

    _tstring strFindName;
    _tstring strFindFile = strSrc;
    _tstring strFindDir = strSrc;

    //文件夹则匹配全部类型文件
    if (IsDirectory(strSrc))
    {
        strFindFile = strSrc + _T("\\*");
    }
    else
    {
        strFindDir = GetFileDir(strSrc);
        strFindName = GetFileName(strSrc, true);
    }

    if (!IsDirectory(strFindDir))
    {
        return false;
    }

    //先拷贝符合规则的文件
    do
    {
        hFindHandle = FindFirstFile(strFindFile.c_str(), &findData);
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            break;
        }

        do
        {
            //上一级目录与当前目录跳过
            if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
            {
                continue;
            }

            //拷贝文件
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                _tstring strPathTo = strDest + _T("\\") + findData.cFileName;
                if (!IsDirectory(strDest))
                {
                    CreateDir(strDest);
                }

                _tstring strFileFrom = strFindDir + _T("\\") + findData.cFileName;

                if (!bOverwrites && IsFile(strPathTo))
                {
                    return true;
                }

                ::CopyFile(strFileFrom.c_str(), strPathTo.c_str(), FALSE);
            }
        } while (::FindNextFile(hFindHandle, &findData));

        ::FindClose(hFindHandle);

    } while (false);

    //后拷贝文件夹
    do
    {
        hFindHandle = FindFirstFile((strFindDir + _T("\\*")).c_str(), &findData);
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            break;
        }

        do
        {
            //上一级目录与当前目录跳过
            if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
            {
                continue;
            }

            //拷贝文件
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                _tstring strPathTo = strDest + _T("\\") + findData.cFileName;
                if (bEmptyDir && !IsDirectory(strPathTo))
                {
                    CreateDir(strPathTo);
                }

                _tstring strFileFrom = strFindDir + _T("\\") + findData.cFileName;

                if (strFindName.empty())
                {
                    CopyDir(strFileFrom, strPathTo, bEmptyDir, bOverwrites, depth - 1);
                }
                else
                {
                    CopyDir(strFileFrom + _T("\\") + strFindName, strPathTo, bEmptyDir, bOverwrites, depth - 1);
                }
            }

        } while (::FindNextFile(hFindHandle, &findData));

        ::FindClose(hFindHandle);

    } while (false);

    return true;
}

bool CPathUtils::Delete(const _tstring& strPath)
{
    if (IsFile(strPath))
    {
        return DeleteArchive(strPath);
    }

    if (IsDirectory(strPath))
    {
        return DeleteDir(strPath);
    }

    return false;
}

bool CPathUtils::DeleteArchive(const _tstring& strPath)
{
    if (strPath.empty())
    {
        return false;
    }

    return ::DeleteFile(strPath.c_str());
}

bool CPathUtils::DeleteDir(const _tstring& strResult)
{
    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;

    if (strResult.empty())
    {
        return false;
    }

    hFindHandle = FindFirstFile((strResult + _T("\\*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return false;
    }

    do
    {
        _tstring strFilePath = strResult + _T("\\") + findData.cFileName;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //删除目录
        {
            DeleteDir(strFilePath);
            continue;
        }
        else    //删除文件
        {
            ::DeleteFile(strFilePath.c_str());
        }

    } while (::FindNextFile(hFindHandle, &findData));
    ::FindClose(hFindHandle);

    return ::RemoveDirectory(strResult.c_str());
}

bool CPathUtils::CreateDir(const _tstring& strPath)
{
    _tstring strDriver;              //驱动器号, 如 D:
    _tstring strSubPath = strPath;   //路径, 如 Test\1\2\3

    if (strPath.empty())
    {
        return false;
    }

    //获取盘符
    do
    {
        size_t nFindIndex = strPath.find_first_of(':');  //检查是否有驱动器号
        if (nFindIndex == _tstring::npos)
        {
            break;
        }

        strDriver = strPath.substr(0, nFindIndex + 1); //得到驱动器号, 如 D:
        nFindIndex = strPath.find(_T("\\"), nFindIndex);
        if (nFindIndex == _tstring::npos)
        {
            break;
        }

        strSubPath = strPath.substr(nFindIndex + 1); //得到路径, 如 Test\1\2\3
    } while (false);

    _tstring strDestDir;
    size_t nFindBegin = 0;
    size_t nFindIndex = 0;
    do
    {
        nFindIndex = strSubPath.find(_T("\\"), nFindBegin);
        if (nFindIndex != _tstring::npos)
        {
            strDestDir = strSubPath.substr(0, nFindIndex);
            nFindBegin = nFindIndex + 1;
        }
        else
        {
            strDestDir = strSubPath;
        }

        if (!strDriver.empty())
        {
            strDestDir = strDriver + _T("\\") + strDestDir;
        }

        if (!::CreateDirectory(strDestDir.c_str(), NULL) && ERROR_ALREADY_EXISTS != ::GetLastError())
        {
            return false;
        }

    } while (nFindIndex != _tstring::npos);

    return true;
}

int CPathUtils::SHCopy(const _tstring& strSrc, const _tstring& strDest)
{
    _tstring strFrom = GetAbsolutePath(strSrc) + _T('\0');
    _tstring strTo = GetAbsolutePath(strDest) + _T('\0');

    SHFILEOPSTRUCT sfos = { 0 };
    sfos.wFunc = FO_COPY;
    sfos.pFrom = strFrom.c_str();   //注意 此字符串必须以双 null 结尾
    sfos.pTo = strTo.c_str();       //注意 此字符串必须以双 null 结尾
    sfos.fFlags = FOF_NO_UI | FOF_FILESONLY;

    return SHFileOperation(&sfos);
}

int CPathUtils::SHMove(const _tstring& strSrc, const _tstring& strDest)
{
    _tstring strFrom = GetAbsolutePath(strSrc) + _T('\0');
    _tstring strTo = GetAbsolutePath(strDest) + _T('\0');

    SHFILEOPSTRUCT sfos = { 0 };
    sfos.wFunc = FO_MOVE;
    sfos.pFrom = strFrom.c_str();   //注意 此字符串必须以双 null 结尾
    sfos.pTo = strTo.c_str();       //注意 此字符串必须以双 null 结尾
    sfos.fFlags = FOF_NO_UI;

    return SHFileOperation(&sfos);
}

int CPathUtils::SHRename(const _tstring& strSrc, const _tstring& strDest)
{
    _tstring strFrom = GetAbsolutePath(strSrc) + _T('\0');
    _tstring strTo = GetAbsolutePath(strDest) + _T('\0');

    SHFILEOPSTRUCT sfos = { 0 };
    sfos.wFunc = FO_RENAME;
    sfos.pFrom = strFrom.c_str();   //注意 此字符串必须以双 null 结尾
    sfos.pTo = strTo.c_str();       //注意 此字符串必须以双 null 结尾
    sfos.fFlags = FOF_NO_UI;

    return SHFileOperation(&sfos);
}

int CPathUtils::SHDelete(const _tstring& strSrc)
{
    _tstring strFrom = GetAbsolutePath(strSrc) + _T('\0');

    SHFILEOPSTRUCT sfos = { 0 };
    sfos.wFunc = FO_DELETE;
    sfos.pFrom = strFrom.c_str();   //注意 此字符串必须以双 null 结尾
    sfos.fFlags = FOF_NO_UI;

    return SHFileOperation(&sfos);
}

_tstring CPathUtils::GetExpandEnvStrings(const _tstring& strPath)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::ExpandEnvironmentStrings(strPath.c_str(), szBuf, _countof(szBuf));
    return szBuf;
}

_tstring CPathUtils::GetUnExpandEnvStrings(const _tstring& strPath)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::PathUnExpandEnvStrings(strPath.c_str(), szBuf, _countof(szBuf));
    return szBuf;
}

std::vector<_tstring> CPathUtils::GetFileList(
    const _tstring& strDir,
    const std::vector<_tstring>& extList /* = {} */,
    bool bIncludeHide/* = false*/,
    int32_t depth /*  = MAX_PATH */,
    bool bIgnoreCase/* = true*/
)
{
    std::vector<_tstring> fileList;
    _tstring strSrcDir = GetAbsolutePath(strDir);
    _GetFileList(fileList, strSrcDir, extList, bIncludeHide, depth, bIgnoreCase);
    return fileList;
}

std::vector<_tstring> CPathUtils::GetFileList(
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    std::vector<_tstring> fileList;
    _tstring strSrcDir = GetAbsolutePath(strDir);
    _GetFileList(fileList, strSrcDir, depth, dwExclude, dwEontain);
    return fileList;
}

std::vector<FILEINFO> CPathUtils::GetFileListEx(
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    std::vector<FILEINFO> fileList;
    _tstring strSrcDir = GetAbsolutePath(strDir);
    _GetFileListEx(fileList, strSrcDir, depth, dwExclude, dwEontain);
    return fileList;
}

std::vector<_tstring> CPathUtils::SearchFiles(
    const _tstring& strFile,
    const _tstring& strSrcName,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    std::vector<_tstring> fileList;
    _tstring strSrcDir = GetAbsolutePath(strFile);
    _tstring strName = strSrcName;

    if (strName.empty())
    {
        strName = _T("*");
    }

    if (GetFileDir(strName).empty())
    {
        _SearchFiles(fileList, strSrcDir, strName, depth, dwExclude, dwEontain);
    }
    return fileList;
}

void CPathUtils::_GetFileList(
    std::vector<_tstring>& fileList,
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;
        bool bExclude = false;
        bool bEontain = true;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }
        
        // 检查排除属性
        if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
        {
            bExclude = true;
        }

        // 检查包含属性
        if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
        {
            bEontain = true;
        }

        // 属性检查
        if (!bExclude && bEontain)
        {
            fileList.emplace_back(strDir + _T("\\") + strName);
        }

        //非目录
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            (void)_GetFileList(fileList, strDir + _T("\\") + strName, depth, dwExclude, dwEontain);
            continue;
        }

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CPathUtils::_GetFileListEx(
    std::vector<FILEINFO>& fileList,
    const _tstring& strDir,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        bool bExclude = false;
        bool bEontain = true;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        // 检查排除属性
        if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
        {
            bExclude = true;
        }

        // 检查包含属性
        if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
        {
            bEontain = true;
        }

        // 属性检查
        if (!bExclude && bEontain)
        {
            FILEINFO info;

            info.m_filePath = strDir + _T("\\") + findData.cFileName;
            info.m_fileSize = (unsigned long long)findData.nFileSizeHigh << 32 | findData.nFileSizeLow;
            info.m_ftCreationTime = *(size_t*)&findData.ftCreationTime;
            info.m_ftLastAccessTime = *(size_t*)&findData.ftLastAccessTime;
            info.m_ftLastWriteTime = *(size_t*)&findData.ftLastWriteTime;
            info.m_fileAttributes = findData.dwFileAttributes;

            info.m_ftCreationTime = (info.m_ftCreationTime - 116444736000000000) / 10000000;
            info.m_ftLastAccessTime = (info.m_ftLastAccessTime - 116444736000000000) / 10000000;
            info.m_ftLastWriteTime = (info.m_ftLastWriteTime - 116444736000000000) / 10000000;

            fileList.emplace_back(info);
        }

        //非目录
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            (void)_GetFileListEx(fileList, strDir + _T("\\") + findData.cFileName, depth, dwExclude, dwEontain);
            continue;
        }

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CPathUtils::_SearchFiles(
    std::vector<_tstring>& fileList,
    const _tstring& strSrcDir,
    const _tstring& strSrcName,
    int32_t depth/* = MAX_PATH*/,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;

    if (!IsDirectory(strSrcDir))
    {
        return;
    }

    //先搜索符合规则的文件
    do
    {
        _tstring strFindFile = strSrcDir + _T("\\") + strSrcName;
        hFindHandle = FindFirstFile(strFindFile.c_str(), &findData);
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            break;
        }

        do
        {
            bool bExclude = false;
            bool bEontain = true;

            //上一级目录与当前目录跳过
            if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
            {
                continue;
            }

            // 检查排除属性
            if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
            {
                bExclude = true;
            }

            // 检查包含属性
            if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
            {
                bEontain = true;
            }

            // 属性检查
            if (!bExclude && bEontain && (0 == (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
            {
                fileList.emplace_back(strSrcDir + _T("\\") + findData.cFileName);
            }

        } while (::FindNextFile(hFindHandle, &findData));

        ::FindClose(hFindHandle);

    } while (false);

    //后搜索文件夹
    do
    {
        _tstring strFindFile = strSrcDir + _T("\\*");
        hFindHandle = FindFirstFile(strFindFile.c_str(), &findData);
        if (INVALID_HANDLE_VALUE == hFindHandle)
        {
            break;
        }

        do
        {
            bool bExclude = false;
            bool bEontain = true;

            //上一级目录与当前目录跳过
            if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
            {
                continue;
            }

            // 检查排除属性
            if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
            {
                bExclude = true;
            }

            // 检查包含属性
            if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
            {
                bEontain = true;
            }

            // 属性检查
            if (!bExclude && bEontain && (0 != (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
            {
                fileList.emplace_back(strSrcDir + _T("\\") + findData.cFileName);
            }

            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                _SearchFiles(fileList, strSrcDir + _T("\\") + findData.cFileName, strSrcName, depth, dwExclude, dwEontain);
            }

        } while (::FindNextFile(hFindHandle, &findData));

        ::FindClose(hFindHandle);

    } while (false);
}

void CPathUtils::_GetFileList(
    std::vector<_tstring>& fileList,
    const _tstring& strDir,
    const std::vector<_tstring>& extList /* = {} */,
    int32_t depth /*  = MAX_PATH */,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((strDir + _T("\\*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        bool bExclude = false;
        bool bEontain = true;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        // 检查排除属性
        if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
        {
            bExclude = true;
        }

        // 检查包含属性
        if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
        {
            bEontain = true;
        }

        // 属性检查
        if (!bExclude && bEontain)
        {
            if (extList.empty())
            {
                fileList.emplace_back(strDir + _T("\\") + strName);
            }
            else
            {
                //获取扩展名
                _tstring strExtName = GetFileExt(strName, false);

                //进行扩展名过滤
                for (auto& item : extList)
                {
                    if (0 == _tcsicmp(item.c_str(), strExtName.c_str()))
                    {
                        fileList.emplace_back(strDir + _T("\\") + strName);
                    }
                }
            }
        }

        //非目录
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            (void)_GetFileList(fileList, strDir + _T("\\") + strName, extList, depth - 1, dwExclude, dwEontain);
        }

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

void CPathUtils::_GetFileList(
    std::map<_tstring, FILEINFO>& fileList,
    const _tstring& dir,
    const std::vector<_tstring>& extList /* = {} */,
    int32_t depth /*  = MAX_PATH */,
    DWORD dwExclude/* = 0*/,
    DWORD dwEontain/* = 0*/
)
{
    //到达深度限制返回
    if (depth < 0)
    {
        return;
    }

    WIN32_FIND_DATA findData = { 0 };
    HANDLE hFindHandle = INVALID_HANDLE_VALUE;
    hFindHandle = FindFirstFile((dir + _T("\\*.*")).c_str(), &findData);
    if (INVALID_HANDLE_VALUE == hFindHandle)
    {
        return;
    }

    do
    {
        _tstring strName = findData.cFileName;

        bool bExclude = false;
        bool bEontain = true;

        //上一级目录与当前目录跳过
        if (0 == _tcscmp(findData.cFileName, _T(".")) || 0 == _tcscmp(findData.cFileName, _T("..")))
        {
            continue;
        }

        // 检查排除属性
        if (0 != dwExclude && (0 != (findData.dwFileAttributes & dwExclude)))
        {
            bExclude = true;
        }

        // 检查包含属性
        if (0 != dwEontain && (dwEontain == (findData.dwFileAttributes & dwEontain)))
        {
            bEontain = true;
        }

        // 属性检查
        if (!bExclude && bEontain)
        {
            FILEINFO info;
            info.m_filePath = dir + _T("\\") + strName;
            info.m_fileSize = (unsigned long long)findData.nFileSizeHigh << 32 | findData.nFileSizeLow;
            info.m_ftCreationTime = *(size_t*)&findData.ftCreationTime;
            info.m_ftLastAccessTime = *(size_t*)&findData.ftLastAccessTime;
            info.m_ftLastWriteTime = *(size_t*)&findData.ftLastWriteTime;
            info.m_fileAttributes = findData.dwFileAttributes;

            info.m_ftCreationTime = (info.m_ftCreationTime - 116444736000000000) / 10000000;
            info.m_ftLastAccessTime = (info.m_ftLastAccessTime - 116444736000000000) / 10000000;
            info.m_ftLastWriteTime = (info.m_ftLastWriteTime - 116444736000000000) / 10000000;

            if (extList.empty())
            {
                fileList.insert(std::make_pair(dir + _T("\\") + strName, info));
            }
            else
            {
                //获取扩展名
                _tstring strExtName = GetFileExt(strName, false);

                //进行扩展名过滤
                for (const auto& item : extList)
                {
                    if (0 == _tcsicmp(item.c_str(), strExtName.c_str()))
                    {
                        fileList.insert(std::make_pair(dir + _T("\\") + strName, info));
                    }
                }
            }
        }

        //目录
        if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            (void)_GetFileList(fileList, dir + _T("\\") + strName, extList, depth - 1, dwExclude, dwEontain);
        }

    } while (::FindNextFile(hFindHandle, &findData));

    ::FindClose(hFindHandle);
}

ULONGLONG CPathUtils::GetDiskFreeSize(const _tstring& strDir)
{
    ULARGE_INTEGER ullFreeBytesAvailableToCaller = { 0 };
    ULARGE_INTEGER ullTotalNumberOfBytes = { 0 };
    ULARGE_INTEGER ullTotalNumberOfFreeBytes = { 0 };

    ::GetDiskFreeSpaceEx(strDir.c_str(), 
        &ullFreeBytesAvailableToCaller,
        &ullTotalNumberOfBytes,
        &ullTotalNumberOfFreeBytes
        );

    return ullTotalNumberOfFreeBytes.QuadPart;
}

ULONGLONG CPathUtils::GetDiskTotalSize(const _tstring& strDir)
{
    ULARGE_INTEGER ullFreeBytesAvailableToCaller = { 0 };
    ULARGE_INTEGER ullTotalNumberOfBytes = { 0 };
    ULARGE_INTEGER ullTotalNumberOfFreeBytes = { 0 };

    ::GetDiskFreeSpaceEx(strDir.c_str(),
        &ullFreeBytesAvailableToCaller,
        &ullTotalNumberOfBytes,
        &ullTotalNumberOfFreeBytes
    );

    return ullTotalNumberOfBytes.QuadPart;
}