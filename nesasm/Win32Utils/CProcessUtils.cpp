#include "CProcessUtils.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <strsafe.h>
#include <stdint.h>

#define DPSAPI_VERSION = 1
#pragma comment(lib, "Psapi.lib")

bool CProcessUtils::RunProcess(
    const _tstring& strCmd,
    const _tstring& strCurDir,
    bool bShow,
    bool bWait
)
{
    TCHAR szCommandLine[MAX_PATH] = { 0 };
    SECURITY_ATTRIBUTES   sa = { 0 };
    STARTUPINFO si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    LPCTSTR lpCurrentDir = NULL;

    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;
    sa.nLength = sizeof(sa);

    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = bShow ? SW_SHOW : SW_HIDE;
    si.hStdInput = NULL;
    si.hStdOutput = NULL;
    si.hStdError = NULL;

    if (!strCmd.empty())
    {
        ::StringCchCopy(szCommandLine, _countof(szCommandLine), strCmd.c_str());
    }

    if (!strCurDir.empty())
    {
        lpCurrentDir = strCurDir.c_str();
    }

    if (!::CreateProcess(NULL, szCommandLine, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, lpCurrentDir, &si, &pi))
    {
        return false;
    }

    if (bWait)
    {
        ::WaitForSingleObject(pi.hProcess, INFINITE);
    }

    ::CloseHandle(pi.hProcess);
    ::CloseHandle(pi.hThread);

    return true;
}

_tstring CProcessUtils::GetPath(DWORD dwPID, bool bNtPath)
{
    _tstring strPath;
    HANDLE hProcess = NULL;
    TCHAR szBuffer[MAX_PATH] = { 0 };
    DWORD dwSize = _countof(szBuffer);

    do
    {
        hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPID);
        if (NULL == hProcess)
        {
            break;
        }

        if (0 == ::GetProcessImageFileName(hProcess, szBuffer, dwSize))
        {
            break;
        }

        if (bNtPath)
        {
            strPath = DosPathToNtPath(szBuffer);
        }
        else
        {
            strPath = szBuffer;
        }

    } while (false);

    if (NULL != hProcess)
    {
        ::CloseHandle(hProcess);
    }

    return strPath;
}

_tstring CProcessUtils::GetPathEx(DWORD dwPID, bool bNtPath)
{
    _tstring strPath;
    HANDLE hProcess = NULL;
    TCHAR szBuffer[MAX_PATH] = { 0 };
    DWORD dwSize = _countof(szBuffer);

    do
    {
        hProcess = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwPID);
        if (NULL == hProcess)
        {
            break;
        }

        if (!::QueryFullProcessImageName(hProcess, bNtPath ? 0 : PROCESS_NAME_NATIVE, szBuffer, &dwSize))
        {
            break;
        }

        strPath = szBuffer;

    } while (false);

    if (NULL != hProcess)
    {
        ::CloseHandle(hProcess);
    }

    return strPath;
}

_tstring CProcessUtils::GetParentPath(DWORD dwPID, bool bNtPath)
{
    _tstring strPath;
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);

    do
    {
        hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcessSnap)
        {
            break;
        }

        if (!::Process32First(hProcessSnap, &pe32))
        {
            break;
        }

        do
        {
            if (pe32.th32ProcessID == dwPID)
            {
                strPath = GetPath(pe32.th32ParentProcessID, bNtPath);
                break;
            }

        } while (::Process32Next(hProcessSnap, &pe32));

    } while (false);

    if (INVALID_HANDLE_VALUE != hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return strPath;
}

DWORD CProcessUtils::GetParentID(DWORD dwPID)
{
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);
    DWORD dwParentProcessId = 0;

    do
    {
        hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcessSnap)
        {
            break;
        }

        if (!::Process32First(hProcessSnap, &pe32))
        {
            break;
        }

        do
        {
            if (pe32.th32ProcessID == dwPID)
            {
                dwParentProcessId = pe32.th32ParentProcessID;
                break;
            }

        } while (::Process32Next(hProcessSnap, &pe32));

    } while (false);

    if (INVALID_HANDLE_VALUE != hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return dwParentProcessId;
}

std::vector<_tstring> CProcessUtils::GetModules(DWORD dwPID)
{
    std::vector<_tstring> modulesList;
    TCHAR szModName[MAX_PATH] = { 0 };
    HMODULE* phMods = NULL;
    HANDLE hProcess = NULL;
    DWORD cbNeeded = 0;
    DWORD dwModSize = 0;

    do
    {
        hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
        if (NULL == hProcess)
        {
            break;
        }

        if (!::EnumProcessModulesEx(hProcess, NULL, NULL, &cbNeeded, LIST_MODULES_ALL))
        {
            break;
        }

        dwModSize = cbNeeded;

        phMods = (HMODULE*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwModSize);
        if (NULL == phMods)
        {
            break;
        }

        if (!::EnumProcessModulesEx(hProcess, phMods, dwModSize, &cbNeeded, LIST_MODULES_ALL))
        {
            break;
        }

        size_t nModuleCnt = (cbNeeded / sizeof(HMODULE));
        for (size_t i = 0; i < nModuleCnt; i++)
        {
            if (::GetModuleFileNameEx(hProcess, phMods[i], szModName, _countof(szModName)))
            {
                modulesList.push_back(szModName);
            }
        }

    } while (false);

    if (NULL != hProcess)
    {
        ::CloseHandle(hProcess);
    }

    if (phMods)
    {
        ::HeapFree(::GetProcessHeap(), 0, phMods);
    }

    return modulesList;
}

bool CProcessUtils::GetPEHeader(DWORD dwPID, PE_HEADER_INFO* pPEHeader)
{
    HMODULE hModule = NULL;
    LPVOID OldValue = NULL;
    BOOL isDisableWow64Fs = ::Wow64DisableWow64FsRedirection(&OldValue);
    bool fSuccess = false;

    if (nullptr == pPEHeader)
    {
        return false;
    }

    do
    {
        DWORD dwFlags = LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE;
        hModule = ::LoadLibraryEx(
            GetPath(dwPID).c_str(),
            0,
            dwFlags
        );

        if (NULL == hModule)
        {
            break;
        }

        LPBYTE pHeader = (BYTE*)hModule;
        BYTE* pImageData = (BYTE*)((ULONG_PTR)pHeader & ~((ULONG_PTR)0x03));
        PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageData;
        if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        {
            break;
        }

        PIMAGE_NT_HEADERS pNtHeader = (IMAGE_NT_HEADERS*)((BYTE*)(pDosHeader)+(DWORD)(pDosHeader->e_lfanew));
        if (IMAGE_NT_SIGNATURE != pNtHeader->Signature)
        {
            break;
        }

        // 检查 是否为 32位程序可选头
        if (IMAGE_NT_OPTIONAL_HDR32_MAGIC == pNtHeader->OptionalHeader.Magic)
        {
            pPEHeader->m_NtHeadersMagic = IMAGE_NT_OPTIONAL_HDR32_MAGIC;
            pPEHeader->m_NtHeaders32 = *(PIMAGE_NT_HEADERS32)pNtHeader;
        }
        // 检查 是否为 64位程序可选头
        else if (IMAGE_NT_OPTIONAL_HDR64_MAGIC == pNtHeader->OptionalHeader.Magic)
        {
            pPEHeader->m_NtHeadersMagic = IMAGE_NT_OPTIONAL_HDR64_MAGIC;
            pPEHeader->m_NtHeaders64 = *(PIMAGE_NT_HEADERS64)pNtHeader;
        }
        else
        {
            break;
        }

        pPEHeader->m_DosHeader = *(PIMAGE_DOS_HEADER)pImageData;
        fSuccess = true;

    } while (false);

    if (isDisableWow64Fs)
    {
        ::Wow64RevertWow64FsRedirection(OldValue);
    }

    if (NULL != hModule)
    {
        ::FreeLibrary(hModule);
    }

    return fSuccess;
}

bool CProcessUtils::WaitForProcess(
    DWORD dwPID,
    DWORD dwMilliseconds/* = INFINITE*/
)
{
    HANDLE hProcess = NULL;
    bool fResult = false;
        
    hProcess = ::OpenProcess(SYNCHRONIZE, FALSE, dwPID);
    if (NULL == hProcess)
    {
        return false;
    }

    fResult = (WAIT_OBJECT_0 == ::WaitForSingleObject(hProcess, dwMilliseconds));
    ::CloseHandle(hProcess);

    return fResult;
}

std::map<DWORD, PROC_INFO> CProcessUtils::GetAllProcessInfos()
{
    std::map<DWORD, PROC_INFO> infos;
    HANDLE hProcessSnap = NULL;
    PROCESSENTRY32 pe32 = { 0 };
    pe32.dwSize = sizeof(pe32);

    do
    {
        hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (INVALID_HANDLE_VALUE == hProcessSnap)
        {
            break;
        }

        if (!::Process32First(hProcessSnap, &pe32))
        {
            break;
        }

        do
        {
            PROC_INFO info;
            info.dwParentPID = pe32.th32ParentProcessID;
            info.dwPID = pe32.th32ProcessID;
            info.strExeFile = pe32.szExeFile;
            info.cntThreads = pe32.cntThreads;
            info.nPriClassBase = pe32.pcPriClassBase;

            infos.insert(std::make_pair(info.dwPID, info));

        } while (::Process32Next(hProcessSnap, &pe32));

    } while (false);

    if (INVALID_HANDLE_VALUE != hProcessSnap)
    {
        ::CloseHandle(hProcessSnap);
    }

    return infos;
}

std::vector<MODULE_INFO> CProcessUtils::GetModuleInfos(DWORD dwPID)
{
    std::vector<MODULE_INFO> vModules;
    HANDLE hModuleSnap = NULL;
    MODULEENTRY32 me32 = { 0 };
    me32.dwSize = sizeof(me32);

    do
    {
        hModuleSnap = ::CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32,
            dwPID
        );

        if (INVALID_HANDLE_VALUE == hModuleSnap)
        {
            break;
        }

        if (!::Module32First(hModuleSnap, &me32))
        {
            break;
        }

        do
        {
            MODULE_INFO info;
            info.modBaseSize = me32.modBaseSize;
            info.strExePath = me32.szExePath;
            info.strModule = me32.szModule;
            info.hModule = me32.hModule;
            info.modBaseAddr = me32.modBaseAddr;

            vModules.push_back(info);
        } while (::Module32Next(hModuleSnap, &me32));

    } while (false);

    if (INVALID_HANDLE_VALUE != hModuleSnap)
    {
        ::CloseHandle(hModuleSnap);
    }

    return vModules;
}

PROCESS_TREE CProcessUtils::GetProcessTree(DWORD dwPID)
{
    std::map<DWORD, PROC_INFO> procInfos = CProcessUtils::GetAllProcessInfos();
    return _GetProcessTree(procInfos, dwPID);
}

PROCESS_TREE CProcessUtils::_GetProcessTree(
    const std::map<DWORD, PROC_INFO>& procInfos,
    DWORD dwPID
)
{
    PROCESS_TREE procTree;
    for (auto& item : procInfos)
    {
        const PROC_INFO& info = item.second;
        if (item.first == dwPID)
        {
            procTree.dwPID = dwPID;
            procTree.strExeFile = info.strExeFile;
            procTree.nPriClassBase = info.nPriClassBase;
            procTree.dwCntThreads = info.cntThreads;
            procTree.dwParentPID = info.dwParentPID;
        }
        else if (info.dwParentPID == dwPID)
        {
            PROCESS_TREE subTree = _GetProcessTree(procInfos, info.dwPID);
            procTree.childProcesses.push_back(subTree);
        }
    }

    return procTree;
}

bool CProcessUtils::KillProcess(DWORD dwPID)
{
    HANDLE hProcess = NULL;
    BOOL fSuccess = false;

    // 杀死进程
    hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, dwPID);
    if (NULL == hProcess)
    {
        return false;
    }

    fSuccess = ::TerminateProcess(hProcess, 0);
    ::CloseHandle(hProcess);

    return fSuccess;
}

bool CProcessUtils::KillProcessTree(const PROCESS_TREE& procTree)
{
    HANDLE hProcess = NULL;
    BOOL fSuccess = false;

    // 杀死子进程
    for (auto item : procTree.childProcesses)
    {
        KillProcessTree(item);
    }

    // 杀死进程
    hProcess = ::OpenProcess(PROCESS_TERMINATE, FALSE, procTree.dwPID);
    if (NULL == hProcess)
    {
        return false;
    }

    fSuccess = ::TerminateProcess(hProcess, 0);
    ::CloseHandle(hProcess);

    return fSuccess;
}

bool CProcessUtils::KillProcessTree(DWORD dwPID)
{
    return KillProcessTree(GetProcessTree(dwPID));
}

_tstring CProcessUtils::DosPathToNtPath(const _tstring& strPath)
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

_tstring CProcessUtils::NtPathToDosPath(const _tstring& strPath)
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

DWORD CProcessUtils::GetSubsystemType(DWORD dwPID)
{
    PE_HEADER_INFO peHeader = { 0 };

    if (!GetPEHeader(dwPID, &peHeader))
    {
        return 0;
    }

    if (IMAGE_NT_OPTIONAL_HDR32_MAGIC == peHeader.m_NtHeadersMagic)
    {
        return peHeader.m_NtHeaders32.OptionalHeader.Subsystem;
    }

    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC == peHeader.m_NtHeadersMagic)
    {
        return peHeader.m_NtHeaders64.OptionalHeader.Subsystem;
    }

    return 0;
}

WORD CProcessUtils::GetMachine(DWORD dwPID)
{
    PE_HEADER_INFO peHeader = { 0 };

    if (!GetPEHeader(dwPID, &peHeader))
    {
        return 0;
    }

    if (IMAGE_NT_OPTIONAL_HDR32_MAGIC == peHeader.m_NtHeadersMagic)
    {
        return peHeader.m_NtHeaders32.FileHeader.Machine;
    }

    if (IMAGE_NT_OPTIONAL_HDR64_MAGIC == peHeader.m_NtHeadersMagic)
    {
        return peHeader.m_NtHeaders64.FileHeader.Machine;
    }

    return 0;
}

bool CProcessUtils::IsX86(DWORD dwPID)
{
    return IMAGE_FILE_MACHINE_I386 == GetMachine(dwPID);
}

bool CProcessUtils::IsX64(DWORD dwPID)
{
    return IMAGE_FILE_MACHINE_AMD64 == GetMachine(dwPID);
}

bool CProcessUtils::IsARM32(DWORD dwPID)
{
    WORD wMachine = GetMachine(dwPID);
    return wMachine >= IMAGE_FILE_MACHINE_ARM && wMachine <= IMAGE_FILE_MACHINE_ARMNT;
}

bool CProcessUtils::IsARM64(DWORD dwPID)
{
    return IMAGE_FILE_MACHINE_ARM64 == GetMachine(dwPID);
}

bool CProcessUtils::IsWindowsGUI(DWORD dwPID)
{
    return IMAGE_SUBSYSTEM_WINDOWS_GUI == GetSubsystemType(dwPID);
}

bool CProcessUtils::IsWindowsCUI(DWORD dwPID)
{
    return IMAGE_SUBSYSTEM_WINDOWS_CUI == GetSubsystemType(dwPID);
}
