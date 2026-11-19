#pragma once

#include <atlbase.h>
#include <atlcom.h>
#include <shobjidl.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <map>
#include <functional>

#ifdef UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

namespace CComFileDialog
{
    // 字符串处理工具
    namespace CStrUtils
    {
        std::string WStrToAStr(const std::wstring& str);
        std::wstring AStrToWStr(const std::string& str);
        std::string AStrToU8Str(const std::string& str);
        _tstring AStrToTStr(const std::string& str);
        std::wstring U8StrToWStr(const std::string& str);
        std::string U8StrToAStr(const std::string& str);
        _tstring U8StrToTStr(const std::string& str);
        std::string TStrToAStr(const _tstring& str);
        std::wstring TStrToWStr(const _tstring& str);
        std::string TStrToU8Str(const _tstring& str);
        size_t FindNoCaseA(const std::string& strMain, const std::string& strSub, size_t offset = 0);
        size_t FindNoCaseW(const std::wstring& strMain, const std::wstring& strSub, size_t offset = 0);
        size_t FindNoCase(const _tstring& strMain, const _tstring& strSub, size_t offset = 0);
        size_t FindFirstNotOfNoCaseA(const std::string& strMain, const std::string& strSub, size_t offset = 0);
        size_t FindFirstNotOfNoCaseW(const std::wstring& strMain, const std::wstring& strSub, size_t offset = 0);
        size_t FindFirstNotOfNoCase(const _tstring& strMain, const _tstring& strSub, size_t offset = 0);
        std::vector<std::string> SplitStrA(const std::string& str, const std::string& delim, bool fCase = true);
        std::vector<std::wstring> SplitStrW(const std::wstring& str, const std::wstring& delim, bool fCase = true);
        std::vector<_tstring> SplitStr(const _tstring& str, const _tstring& delim, bool fCase = true);
        std::string& ReplaceNoCaseA(std::string& strMain, const std::string& strSub, const std::string& strTarget);
        std::wstring& ReplaceNoCaseW(std::wstring& strMain, const std::wstring& strSub, const std::wstring& strTarget);
        _tstring& ReplaceNoCase(_tstring& strMain, const _tstring& strSub, const _tstring& strTarget);
        std::string& ReplaceA(std::string& strSrc, const std::string& strFind, const std::string& strReplace, bool fCase = false);
        std::wstring& ReplaceW(std::wstring& strSrc, const std::wstring& strFind, const std::wstring& strReplace, bool fCase = false);
        _tstring& Replace(_tstring& strSrc, const _tstring& strFind, const _tstring& strReplace, bool fCase = false);
    }

    // 路径处理工具
    namespace CPathUtils
    {
        _tstring GetCurrentModulePath();
        _tstring GetCurrentModuleDir();
        _tstring GetCurrentModuleName(bool bHasExt = true);
        _tstring GetFileName(const _tstring& strPath, bool bHasExt = true);
        _tstring GetFileExt(const _tstring& strPath, bool bHasDot = false);
        _tstring GetFileDir(const _tstring& strPath);
        bool IsExist(const _tstring& strPath);
        bool GetAttributes(const _tstring& strPath, DWORD& dwAttr);
        DWORD GetAttributes(const _tstring& strPath);
        bool IsFile(const _tstring& strPath);
        bool IsDirectory(const _tstring& strPath);
    }

    // 文件对话框事件回调类
    class FileDialogEvents : public IFileDialogEvents
    {
    public:
        static HRESULT CreateInstance(FileDialogEvents** ppv);
        STDMETHODIMP QueryInterface(REFIID riid, void** ppv);
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        HRESULT STDMETHODCALLTYPE OnFileOk(__RPC__in_opt IFileDialog* pfd);
        HRESULT STDMETHODCALLTYPE OnFolderChanging(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psiFolder);
        HRESULT STDMETHODCALLTYPE OnFolderChange(__RPC__in_opt IFileDialog* pfd);
        HRESULT STDMETHODCALLTYPE OnSelectionChange(__RPC__in_opt IFileDialog* pfd);
        HRESULT STDMETHODCALLTYPE OnShareViolation(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_SHAREVIOLATION_RESPONSE* pResponse);
        HRESULT STDMETHODCALLTYPE OnTypeChange(__RPC__in_opt IFileDialog* pfd);
        HRESULT STDMETHODCALLTYPE OnOverwrite(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_OVERWRITE_RESPONSE* pResponse);
        void SetOnFileOk(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb);
        void SetOnFolderChanging(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psiFolder)> cb);
        void SetOnFolderChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb);
        void SetOnSelectionChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb);
        void SetOnShareViolation(std::function<STDMETHODIMP(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*)> cb);
        void SetOnTypeChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb);
        void SetOnOverwrite(std::function<STDMETHODIMP(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*)> cb);

    protected:
        FileDialogEvents();
        ~FileDialogEvents();

    private:
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> _CbOnFileOk;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psiFolder)> _CbOnFolderChanging;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> _CbOnFolderChange;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> _CbOnSelectionChange;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_SHAREVIOLATION_RESPONSE* pResponse)> _CbOnShareViolation;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> _CbOnTypeChange;
        std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_OVERWRITE_RESPONSE* pResponse)> _CbOnOverwrite;

    private:
        ULONG m_lRefCount;
    };

    typedef struct _FILTER_SPEC
    {
        std::wstring strName;
        std::wstring strSpec;
        std::vector<_tstring> vSpecList;
    }FILTER_SPEC;

    // 文件对话框过滤器类
    class CDlgFilterSpec
    {
    public:

        CDlgFilterSpec(const CDlgFilterSpec& r) = delete;
        CDlgFilterSpec(std::initializer_list<std::pair<_tstring, _tstring>> init);
        ~CDlgFilterSpec();
        UINT GetSize() const;
        UINT GetFileTypeIndex() const;
        void SetFileTypeIndex(UINT index);
        operator COMDLG_FILTERSPEC* () const;
        std::map<int, FILTER_SPEC> GetFilterSpecList() const;
        const FILTER_SPEC GetFilterSpecInfo(UINT uFileTypeIndex);

    private:

        CDlgFilterSpec& operator = (const CDlgFilterSpec& r) = delete;

    private:

        std::map<int, FILTER_SPEC>  m_mapFilterSpec;
        UINT                        m_uFileTypeIndex;
        UINT                        m_uFileTypeSize;
        COMDLG_FILTERSPEC*          m_pFilterSpec;
    };

    //
    // @brief: 获取打开文件路径
    // @param: hOwner           父窗口句柄, 可为NULL
    // @param: strPath          文件路径
    // @param: filter           文件类型过滤器
    // @ret: 操作结果成功与否
    bool GetOpenPath(HWND hOwner, _tstring& strPath, CDlgFilterSpec& filter);

    //
    // @brief: 获取保存文件路径
    // @param: hOwner           父窗口句柄, 可为NULL
    // @param: strPath          文件路径
    // @param: filter           文件类型过滤器
    // @ret: 操作结果成功与否
    bool GetSavePath(HWND hOwner, _tstring& strPath, CDlgFilterSpec& filter);

    //
    // @brief: 获取选择文件夹路径
    // @param: hOwner           父窗口句柄, 可为NULL
    // @param: strDir           文件夹路径
    // @ret: 操作结果成功与否
    bool GetDirPath(HWND hOwner, _tstring& strDir);

    //
    // @brief: 获取所选文件列表
    // @param: hOwner           父窗口句柄, 可为NULL
    // @param: strPath          保存路径
    // @param: filter           文件类型过滤器
    // @ret: 操作结果成功与否
    std::vector<_tstring> GetFilePathList(HWND hOwner, _tstring& strDir, CDlgFilterSpec& filter);
};

