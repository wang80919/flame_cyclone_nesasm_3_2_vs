#pragma once

#include <wtypesbase.h>
#include <tchar.h>
#include <vector>
#include <map>
#include <string>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

// 进程信息
typedef struct _PROC_INFO
{
    DWORD       dwParentPID;            // 父进程ID
    DWORD       dwPID;                  // 进程ID
    _tstring    strExeFile;             // 可执行文件的名称
    LONG        nPriClassBase;          // 此进程创建的任何线程的基本优先级
    DWORD       cntThreads;             // 进程启动的执行线程
}PROC_INFO;

// 模块信息
typedef struct _MODULE_INFO
{
    _tstring    strExePath;             // 模块路径
    _tstring    strModule;              // 模块名
    HMODULE     hModule;                // 拥有进程上下文中模块的句柄
    BYTE*       modBaseAddr;            // 拥有进程上下文中模块的基址
    DWORD       modBaseSize;            // 模块的大小（以字节为单位）
}MODULE_INFO;

// 进程树结点信息
typedef struct _PROCESS_TREE
{
    DWORD       dwPID;                  // 进程ID
    DWORD       dwParentPID;            // 父进程ID
    _tstring    strExeFile;             // 可执行文件的名称
    LONG        nPriClassBase;          // 此进程创建的任何线程的基本优先级
    DWORD       dwCntThreads;           // 进程启动的执行线程
    std::vector<_PROCESS_TREE> childProcesses;    //子进程列表
}PROCESS_TREE;

// PE文件头信息
typedef struct _PE_HEADER_INFO
{
    IMAGE_DOS_HEADER    m_DosHeader;            //Dos头
    IMAGE_NT_HEADERS32  m_NtHeaders32;          //NT头(32位)
    IMAGE_NT_HEADERS64  m_NtHeaders64;          //NT头(64位)
    WORD                m_NtHeadersMagic;       //NT头魔数
}PE_HEADER_INFO;

class CProcessUtils
{
public:

    //
    // @brief: 运行进程
    // @param: strCmd               命令行
    // @param: strCurDir            程序的当前目录
    // @param: bShow                是否显示程序
    // @param: bWait                是否等待程序直到其退出
    // @ret: bool                   操作是否成功
    static bool RunProcess(
        const _tstring& strCmd = _T(""), 
        const _tstring& strCurDir = _T(""), 
        bool bShow = true, 
        bool bWait = true
    );

    //
    // @brief: 获取进程可执行文件路径
    // @param: dwPID                进程ID
    // @param: bNtPath              是否为Win32 样式的 NT 路径
    // @ret: _tstring               进程可执行文件路径
	static _tstring GetPath(DWORD dwPID, bool bNtPath = true
    );

    //
    // @brief: 获取进程可执行文件路径
    // @param: dwPID                进程ID
    // @param: bNtPath              是否为Win32 样式的 NT 路径
    // @ret: _tstring               进程可执行文件路径
    static _tstring GetPathEx(DWORD dwPID, bool bNtPath = true
    );

    //
    // @brief: 获取父进程可执行文件路径
    // @param: dwPID                进程ID
    // @param: bNtPath              是否为Win32 样式的 NT 路径
    // @ret: _tstring               进程可执行文件路径
    static _tstring GetParentPath(DWORD dwPID, bool bNtPath = true
    );

    //
    // @brief: 获取所有进程信息
    // @ret: std::map<DWORD, PROC_INFO>  进程信息
    static std::map<DWORD, PROC_INFO> GetAllProcessInfos();

    //
    // @brief: 获取指定进程的模块信息(注意: 32位进程不能获取64位进程的模块信息)
    // @param: dwPID                进程ID
    // @ret: std::vector<MODULE_INFO>    模块信息
    static std::vector<MODULE_INFO> GetModuleInfos(DWORD dwPID);

    //
    // @brief: 获取进程加载的模块列表(注意: 32位进程不能获取64位进程的模块信息)
    // @param: dwPID                进程ID
    // @ret: _tstring               模块列表
    static std::vector<_tstring> GetModules(DWORD dwPID);

    //
    // @brief: 获取指定进程的进程树
    // @ret: std::map<DWORD, PROC_INFO>  进程信息
    static PROCESS_TREE GetProcessTree(DWORD dwPID);

    //
    // @brief: 获取父进程的进程ID
    // @param: dwPID                进程ID
    // @ret: DWORD                  父进程的进程ID
	static DWORD GetParentID(DWORD dwPID);

    //
    // @brief: 等待进程结束
    // @param: dwPID                进程ID
    // @param: dwMilliseconds       超时时间
    // @ret: bool                   操作是否成功
    static bool WaitForProcess(
        DWORD dwPID, 
        DWORD dwMilliseconds = INFINITE
    );

    //
    // @brief: 杀死指定进程
    // @param: dwPID                进程ID
    // @ret: bool                   操作是否成功
    static bool KillProcess(DWORD dwPID);

    //
    // @brief: 杀死进程树中的进程
    // @param: procTree             进程树
    // @ret: bool                   操作是否成功
    static bool KillProcessTree(const PROCESS_TREE& procTree);

    //
    // @brief: 杀死指定进程ID的进程树
    // @param: dwPID                进程ID
    // @ret: bool                   操作是否成功
    static bool KillProcessTree(DWORD dwPID);

    //
    // @brief: DOS路径转Nt路径
    // @param: strPath              路径
    // @ret: _tstring               Win32 样式的 NT 路径
    static _tstring DosPathToNtPath(const _tstring& strPath);

    //
    // @brief: Nt路径转DOS路径
    // @param: strPath              路径
    // @ret: _tstring               DOS路径
    static _tstring NtPathToDosPath(const _tstring& strPath);

    //
    // @brief: 获取指定进程的PE头信息
    // @param: dwPID                进程ID
    // @param: pPEHeader            PE头信息缓冲
    // @ret: bool                   操作是否成功
    static bool GetPEHeader(DWORD dwPID, PE_HEADER_INFO* pPEHeader);

    //
    // @brief: 获取进程的子系统类型
    // @param: dwPID                进程ID
    // @param: dwMilliseconds       超时时间
    // @ret: DWORD                  子系统类型
    //                              2: IMAGE_SUBSYSTEM_WINDOWS_GUI (windows 图形用户界面 (GUI) 子系统)
    //                              3: IMAGE_SUBSYSTEM_WINDOWS_CUI (Windows 字符模式用户界面 (CUI) 子系统)
    static DWORD GetSubsystemType(DWORD dwPID);

    //
    // @brief: 获取进程所运行的计算机的体系结构类型
    // @param: dwPID                进程ID
    // @ret: DWORD                  计算机的体系结构类型
    //                              0x014C: IMAGE_FILE_MACHINE_I386 (X86 平台)
    //                              0x8664: IMAGE_FILE_MACHINE_AMD64 (X64 平台)
    static WORD GetMachine(DWORD dwPID);

    //
    // @brief: 进程是否为x86平台
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsX86(DWORD dwPID);

    //
    // @brief: 进程是否为 X64 平台
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsX64(DWORD dwPID);

    //
    // @brief: 进程是否为 ARM32 平台
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsARM32(DWORD dwPID);

    //
    // @brief: 进程是否为 ARM64 平台
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsARM64(DWORD dwPID);

    //
    // @brief: 进程是否运行于windows 图形用户界面 (GUI) 子系统
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsWindowsGUI(DWORD dwPID);

    //
    // @brief: 进程是否运行于Windows 字符模式用户界面 (CUI) 子系统
    // @param: dwPID                进程ID
    // @ret: bool                   检查结果
    static bool IsWindowsCUI(DWORD dwPID);

private:

    //
    // @brief: 获取指定进程的进程树
    // @param: procInfos            进程信息
    // @param: dwPID                进程ID
    // @ret: std::map<DWORD, PROC_INFO>  进程信息
    static PROCESS_TREE _GetProcessTree(
        const std::map<DWORD, PROC_INFO>& procInfos,
        DWORD dwPID
    );
};