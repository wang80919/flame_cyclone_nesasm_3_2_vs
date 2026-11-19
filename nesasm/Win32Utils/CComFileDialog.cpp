#include "CComFileDialog.h"

namespace CComFileDialog
{
    // 字符串转换
    namespace CStrUtils
    {
        std::string _WStrToMultiStr(UINT CodePage, const std::wstring& str)
        {
            //计算缓冲区所需的字节长度
            int cbMultiByte = ::WideCharToMultiByte(CodePage, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
            std::string strResult(cbMultiByte, 0);

            //成功则返回写入到指示的缓冲区的字节数
            size_t nConverted = ::WideCharToMultiByte(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size(), NULL, NULL);

            //调整内容长度
            strResult.resize(nConverted);
            return strResult;
        }

        std::wstring _MultiStrToWStr(UINT CodePage, const std::string& str)
        {
            //计算缓冲区所需的字符长度
            int cchWideChar = ::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, NULL, 0);
            std::wstring strResult(cchWideChar, 0);

            //成功则返回写入到指示的缓冲区的字符数
            size_t nConverted = ::MultiByteToWideChar(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size());

            //调整内容长度
            strResult.resize(nConverted);
            return strResult;
        }

        std::string WStrToAStr(const std::wstring& str)
        {
            return _WStrToMultiStr(CP_ACP, str);
        }

        std::string WStrToU8Str(const std::wstring& str)
        {
            return _WStrToMultiStr(CP_UTF8, str);
        }

        _tstring WStrToTStr(const std::wstring& str)
        {
#ifdef _UNICODE
            return str;
#else
            return _WStrToMultiStr(CP_ACP, str);
#endif
        }

        std::wstring AStrToWStr(const std::string& str)
        {
            return _MultiStrToWStr(CP_ACP, str);
        }

        std::string AStrToU8Str(const std::string& str)
        {
            return WStrToU8Str(AStrToWStr(str));
        }

        _tstring AStrToTStr(const std::string& str)
        {
#ifdef _UNICODE
            return _MultiStrToWStr(CP_ACP, str);
#else
            return str;
#endif
        }

        std::wstring U8StrToWStr(const std::string& str)
        {
            return _MultiStrToWStr(CP_UTF8, str);
        }

        std::string U8StrToAStr(const std::string& str)
        {
            return WStrToAStr(U8StrToWStr(str));
        }

        _tstring U8StrToTStr(const std::string& str)
        {
#ifdef _UNICODE
            return _MultiStrToWStr(CP_UTF8, str);
#else
            return WStrToAStr(U8StrToWStr(str));
#endif
        }

        std::string TStrToAStr(const _tstring& str)
        {
#ifdef _UNICODE
            return _WStrToMultiStr(CP_ACP, str);
#else
            return str;
#endif
        }

        std::wstring TStrToWStr(const _tstring& str)
        {
#ifdef _UNICODE
            return str;
#else
            return AStrToWStr(str);
#endif
        }

        std::string TStrToU8Str(const _tstring& str)
        {
#ifdef _UNICODE
            return WStrToU8Str(str);
#else
            return WStrToU8Str(AStrToWStr(str));
#endif
        }

        size_t FindNoCaseA(const std::string& strMain, const std::string& strSub, size_t offset/* = 0*/)
        {
            size_t findPos = std::string::npos;

            do
            {
                // 主串长度比字串小, 不需要比较
                if (strMain.size() < strSub.size() || offset > (strMain.size() - 1))
                {
                    break;
                }

                std::string strMainTmp = strMain;
                std::string strSubTmp = strSub;

                // 转换大小写
                for (auto& item : strMainTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                for (auto& item : strSubTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                //findPos = strMainTmp.find(strSubTmp, offset);
                const char* pFind = std::strstr(strMainTmp.c_str() + offset, strSubTmp.c_str());
                findPos = pFind ? pFind - strMainTmp.c_str() : findPos;

            } while (false);

            return findPos;
        }

        size_t FindNoCaseW(const std::wstring& strMain, const std::wstring& strSub, size_t offset/* = 0*/)
        {
            size_t findPos = std::wstring::npos;

            do
            {
                // 主串长度比字串小, 不需要比较
                if (strMain.size() < strSub.size() || offset > (strMain.size() - 1))
                {
                    break;
                }

                std::wstring strMainTmp = strMain;
                std::wstring strSubTmp = strSub;

                // 转换大小写
                for (auto& item : strMainTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                for (auto& item : strSubTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                //findPos = strMainTmp.find(strSubTmp, offset);
                const wchar_t* pFind = std::wcsstr(strMainTmp.c_str() + offset, strSubTmp.c_str());
                findPos = pFind ? pFind - strMainTmp.c_str() : findPos;

            } while (false);

            return findPos;
        }

        size_t FindNoCase(const _tstring& strMain, const _tstring& strSub, size_t offset/* = 0*/)
        {
#ifdef _UNICODE
            return FindNoCaseW(strMain, strSub, offset);
#else
            return FindNoCaseA(strMain, strSub, offset);
#endif
        }

        size_t FindFirstNotOfNoCaseA(const std::string& strMain, const std::string& strSub, size_t offset/* = 0*/)
        {
            size_t findPos = std::string::npos;

            do
            {
                // 主串长度比字串小, 不需要比较
                if (strMain.size() < strSub.size())
                {
                    break;
                }

                std::string strMainTmp = strMain;
                std::string strSubTmp = strSub;

                // 转换大小写
                for (auto& item : strMainTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                for (auto& item : strSubTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                findPos = strMainTmp.find_first_not_of(strSubTmp, offset);

            } while (false);

            return findPos;
        }

        size_t FindFirstNotOfNoCaseW(const std::wstring& strMain, const std::wstring& strSub, size_t offset/* = 0*/)
        {
            size_t findPos = std::wstring::npos;

            do
            {
                // 主串长度比字串小, 不需要比较
                if (strMain.size() < strSub.size())
                {
                    break;
                }

                std::wstring strMainTmp = strMain;
                std::wstring strSubTmp = strSub;

                // 转换大小写
                for (auto& item : strMainTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                for (auto& item : strSubTmp)
                {
                    if (item >= _T('A') && item <= _T('Z')) item += 0x20;
                }

                findPos = strMainTmp.find_first_not_of(strSubTmp, offset);

            } while (false);

            return findPos;
        }

        size_t FindFirstNotOfNoCase(const _tstring& strMain, const _tstring& strSub, size_t offset/* = 0*/)
        {
#ifdef _UNICODE
            return FindFirstNotOfNoCaseW(strMain, strSub, offset);
#else
            return FindFirstNotOfNoCaseA(strMain, strSub, offset);
#endif
        }

        std::vector<std::string> SplitStrA(const std::string& str, const std::string& delim, bool fCase /* = true*/)
        {
            std::vector<std::string> vectorOut;
            size_t iStart = 0;
            size_t iEnd = 0;

            if (delim.empty())
            {
                vectorOut.push_back(str);
            }
            else
            {
                if (fCase)
                {
                    while ((iStart = str.find_first_not_of(delim, iEnd)) != std::string::npos)
                    {
                        iEnd = str.find(delim, iStart);
                        vectorOut.push_back(str.substr(iStart, iEnd - iStart));
                    }
                }
                else
                {
                    while ((iStart = FindFirstNotOfNoCaseA(str, delim, iEnd)) != std::string::npos)
                    {
                        iEnd = FindNoCaseA(str, delim, iStart);
                        vectorOut.push_back(str.substr(iStart, iEnd - iStart));
                    }
                }
            }

            return vectorOut;
        }

        std::vector<std::wstring> SplitStrW(const std::wstring& str, const std::wstring& delim, bool fCase /* = true*/)
        {
            std::vector<std::wstring> vectorOut;
            size_t iStart = 0;
            size_t iEnd = 0;

            if (delim.empty())
            {
                vectorOut.push_back(str);
            }
            else
            {
                if (fCase)
                {
                    while ((iStart = str.find_first_not_of(delim, iEnd)) != std::wstring::npos)
                    {
                        iEnd = str.find(delim, iStart);
                        vectorOut.push_back(str.substr(iStart, iEnd - iStart));
                    }
                }
                else
                {
                    while ((iStart = FindFirstNotOfNoCaseW(str, delim, iEnd)) != std::string::npos)
                    {
                        iEnd = FindNoCaseW(str, delim, iStart);
                        vectorOut.push_back(str.substr(iStart, iEnd - iStart));
                    }
                }
            }

            return vectorOut;
        }

        std::vector<_tstring> SplitStr(const _tstring& str, const _tstring& delim, bool fCase /* = true*/)
        {
#ifdef _UNICODE
            return SplitStrW(str, delim, fCase);
#else
            return SplitStrA(str, delim, fCase);
#endif
        }

        std::string& ReplaceNoCaseA(std::string& strMain, const std::string& strSub, const std::string& strTarget)
        {
            if (strSub.empty())
            {
                return strMain;
            }

            size_t nFind = 0;
            while (_tstring::npos != (nFind = FindNoCaseA(strMain, strSub, nFind)))
            {
                strMain.replace(nFind, strSub.size(), strTarget);
                nFind += strTarget.size();
            };

            return strMain;
        }

        std::wstring& ReplaceNoCaseW(std::wstring& strMain, const std::wstring& strSub, const std::wstring& strTarget)
        {
            if (strSub.empty())
            {
                return strMain;
            }

            size_t nFind = 0;
            while (_tstring::npos != (nFind = FindNoCaseW(strMain, strSub, nFind)))
            {
                strMain.replace(nFind, strSub.size(), strTarget);
                nFind += strTarget.size();
            };

            return strMain;
        }

        _tstring& ReplaceNoCase(_tstring& strMain, const _tstring& strSub, const _tstring& strTarget)
        {
#ifdef _UNICODE
            return ReplaceNoCaseW(strMain, strSub, strTarget);
#else
            return ReplaceNoCaseA(strMain, strSub, strTarget);
#endif
        }

        std::string& ReplaceA(std::string& strMain, const std::string& strSub, const std::string& strTarget, bool fCase/* = false*/)
        {
            if (strSub.empty())
            {
                return strMain;
            }

            if (fCase)
            {
                size_t nFind = 0;
                while (std::string::npos != (nFind = strMain.find(strSub, nFind)))
                {
                    strMain.replace(nFind, strSub.size(), strTarget);
                    nFind += strTarget.size();
                }
            }
            else
            {
                return ReplaceNoCaseA(strMain, strSub, strTarget);
            }

            return strMain;
        }

        std::wstring& ReplaceW(std::wstring& strMain, const std::wstring& strSub, const std::wstring& strTarget, bool fCase/* = false*/)
        {
            if (strSub.empty())
            {
                return strMain;
            }

            if (fCase)
            {
                size_t nFind = 0;
                while (std::wstring::npos != (nFind = strMain.find(strSub, nFind)))
                {
                    strMain.replace(nFind, strSub.size(), strTarget);
                    nFind += strTarget.size();
                };
            }
            else
            {
                return ReplaceNoCaseW(strMain, strSub, strTarget);
            }

            return strMain;
        }

        _tstring& Replace(_tstring& strMain, const _tstring& strSub, const _tstring& strTarget, bool fCase/* = false*/)
        {
#ifdef _UNICODE
            return ReplaceW(strMain, strSub, strTarget, fCase);
#else
            return ReplaceA(strMain, strSub, strTarget, fCase);
#endif
        }
    }

    namespace CPathUtils
    {
        _tstring GetCurrentModulePath()
        {
            TCHAR szCurPath[MAX_PATH] = { 0 };
            ::GetModuleFileName(NULL, szCurPath, _countof(szCurPath));

            _tstring strResult = szCurPath;
            return strResult;
        }

        _tstring GetCurrentModuleDir()
        {
            return GetFileDir(GetCurrentModulePath());
        }

        _tstring GetCurrentModuleName(bool bHasExt/* = true*/)
        {
            return GetFileName(GetCurrentModulePath(), bHasExt);
        }

        _tstring GetFileName(const _tstring& strPath, bool bHasExt/* = true*/)
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

        _tstring GetFileExt(const _tstring& strPath, bool bHasDot/* = false*/)
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

        _tstring GetFileDir(const _tstring& strPath)
        {
            _tstring strResult;
            size_t nIndex = strPath.find_last_of(_T('\\'));
            if (nIndex != _tstring::npos)
            {
                strResult = strPath.substr(0, nIndex);
            }

            return strResult;
        }

        bool IsExist(const _tstring& strPath)
        {
            WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
            if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
            {
                return false;
            }

            return true;
        }

        bool GetAttributes(const _tstring& strPath, DWORD& dwAttr)
        {
            WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
            if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
            {
                return false;
            }

            dwAttr = attr.dwFileAttributes;
            return true;
        }

        DWORD GetAttributes(const _tstring& strPath)
        {
            WIN32_FILE_ATTRIBUTE_DATA attr = { 0 };
            if (!::GetFileAttributesEx(strPath.c_str(), GetFileExInfoStandard, &attr))
            {
                return 0;
            }

            return attr.dwFileAttributes;
        }

        bool IsFile(const _tstring& strPath)
        {
            DWORD dwAttr = 0;
            if (!GetAttributes(strPath, dwAttr))
            {
                return false;
            }

            return 0 == (dwAttr & FILE_ATTRIBUTE_DIRECTORY);
        }

        bool IsDirectory(const _tstring& strPath)
        {
            return 0 != (GetAttributes(strPath) & FILE_ATTRIBUTE_DIRECTORY);
        }
    }


    FileDialogEvents::FileDialogEvents():
        m_lRefCount(0)
    {

    }

    FileDialogEvents::~FileDialogEvents()
    {

    }

    HRESULT FileDialogEvents::CreateInstance(FileDialogEvents** ppv)
    {
        HRESULT hr = S_OK;
        if (ppv == nullptr)
        {
            return E_POINTER;
        }

        *ppv = nullptr;

        FileDialogEvents* pObj = new (std::nothrow) FileDialogEvents();
        if (nullptr == pObj)
        {
            return E_OUTOFMEMORY;
        }

        hr = pObj->QueryInterface(IID_IFileDialogEvents, (void**)ppv);
        if (FAILED(hr))
        {
            delete pObj;
            *ppv = nullptr;
        }

        return hr;
    }

    STDMETHODIMP FileDialogEvents::QueryInterface(REFIID riid, void** ppv)
    {
        if (riid == IID_IUnknown || riid == IID_IFileDialogEvents)
        {
            *ppv = static_cast<IFileDialogEvents*>(this);
            AddRef();
            return S_OK;
        }

        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) FileDialogEvents::AddRef()
    {
        return InterlockedIncrement(&m_lRefCount);
    }

    STDMETHODIMP_(ULONG) FileDialogEvents::Release()
    {
        LONG ref = InterlockedDecrement(&m_lRefCount);
        if (0 == ref)
        {
            delete this;
        }
        return ref;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnFileOk(__RPC__in_opt IFileDialog* pfd)
    {
        return _CbOnFileOk ? _CbOnFileOk(pfd) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnFolderChanging(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psiFolder)
    {
        return _CbOnFolderChanging ? _CbOnFolderChanging(pfd, psiFolder) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnFolderChange(__RPC__in_opt IFileDialog* pfd)
    {
        return _CbOnFolderChange ? _CbOnFolderChange(pfd) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnSelectionChange(__RPC__in_opt IFileDialog* pfd)
    {
        return _CbOnSelectionChange ? _CbOnSelectionChange(pfd) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnShareViolation(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_SHAREVIOLATION_RESPONSE* pResponse)
    {
        return _CbOnShareViolation ? _CbOnShareViolation(pfd, psi, pResponse) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnTypeChange(__RPC__in_opt IFileDialog* pfd)
    {
        return _CbOnTypeChange ? _CbOnTypeChange(pfd) : S_OK;
    }

    HRESULT STDMETHODCALLTYPE FileDialogEvents::OnOverwrite(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psi, __RPC__out FDE_OVERWRITE_RESPONSE* pResponse)
    {
        return _CbOnOverwrite ? _CbOnOverwrite(pfd, psi, pResponse) : S_OK;
    }

    void FileDialogEvents::SetOnFileOk(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb)
    {
        _CbOnFileOk = cb;
    }

    void FileDialogEvents::SetOnFolderChanging(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd, __RPC__in_opt IShellItem* psiFolder)> cb)
    {
        _CbOnFolderChanging = cb;
    }

    void FileDialogEvents::SetOnFolderChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb)
    {
        _CbOnFolderChange = cb;
    }

    void FileDialogEvents::SetOnSelectionChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb)
    {
        _CbOnSelectionChange = cb;
    }

    void FileDialogEvents::SetOnShareViolation(std::function<STDMETHODIMP(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*)> cb)
    {
        _CbOnShareViolation = cb;
    }

    void FileDialogEvents::SetOnTypeChange(std::function<STDMETHODIMP(__RPC__in_opt IFileDialog* pfd)> cb)
    {
        _CbOnTypeChange = cb;
    }

    void FileDialogEvents::SetOnOverwrite(std::function<STDMETHODIMP(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*)> cb)
    {
        _CbOnOverwrite = cb;
    }

    CDlgFilterSpec::CDlgFilterSpec(std::initializer_list<std::pair<_tstring, _tstring>> init) : m_pFilterSpec(nullptr), m_uFileTypeSize(0), m_uFileTypeIndex(0)
    {
        UINT insert_index = 1;
        for (const auto& pair : init) {

            _tstring str_name = pair.first;
            _tstring str_spec = pair.second;

            _tstring str_spec_list = str_spec;
            CStrUtils::Replace(str_spec_list, _T(" "), _T(""));
            CStrUtils::Replace(str_spec_list, _T("*."), _T("."));

            FILTER_SPEC info;
            info.strName = CStrUtils::TStrToWStr(str_name);
            info.strSpec = CStrUtils::TStrToWStr(str_spec);
            info.vSpecList = CStrUtils::SplitStr(str_spec_list, _T(";"));

            m_mapFilterSpec.insert(std::make_pair(insert_index++, info));
        }

        if (!m_mapFilterSpec.empty())
        {
            m_uFileTypeSize = (UINT)m_mapFilterSpec.size();
            m_pFilterSpec = new (std::nothrow) COMDLG_FILTERSPEC[m_uFileTypeSize];

            if (m_pFilterSpec)
            {
                UINT spec_index = 0;
                for (const auto& item : m_mapFilterSpec)
                {
                    m_pFilterSpec[spec_index].pszName = item.second.strName.c_str();
                    m_pFilterSpec[spec_index].pszSpec = item.second.strSpec.c_str();
                    spec_index++;
                }
            }
        }

        // 0号索引填充一个空过滤器
        FILTER_SPEC info;
        m_mapFilterSpec.insert(std::make_pair(0, info));

    }

    CDlgFilterSpec::~CDlgFilterSpec()
    {
        if (m_pFilterSpec)
        {
            delete[] m_pFilterSpec;
        }
    }

    UINT CDlgFilterSpec::GetSize() const
    {
        return m_uFileTypeSize;
    }

    UINT CDlgFilterSpec::GetFileTypeIndex() const
    {
        return m_uFileTypeIndex;
    }

    void CDlgFilterSpec::SetFileTypeIndex(UINT index)
    {
        m_uFileTypeIndex = (index > m_uFileTypeSize) ? 0 : index;
    }

    CDlgFilterSpec::operator COMDLG_FILTERSPEC* () const
    {
        return m_pFilterSpec; 
    }

    std::map<int, FILTER_SPEC> CDlgFilterSpec::GetFilterSpecList() const
    {
        return m_mapFilterSpec;
    }

    const FILTER_SPEC CDlgFilterSpec::GetFilterSpecInfo(UINT uFileTypeIndex)
    {
        if (uFileTypeIndex > m_uFileTypeSize)
        {
            uFileTypeIndex = 0;
        }

        return m_mapFilterSpec[uFileTypeIndex];
    }

    bool GetOpenPath(HWND hOwner, _tstring& strPath, CDlgFilterSpec& filter)
    {
        HRESULT hr = S_OK;
        bool fComInit = false;

        do
        {
            CComPtr<IFileOpenDialog> pFileDlg;
            FILEOPENDIALOGOPTIONS fos = FOS_FILEMUSTEXIST | FOS_STRICTFILETYPES | FOS_DONTADDTORECENT | FOS_NOCHANGEDIR;

            hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                break;
            }

            fComInit = SUCCEEDED(hr);
            if (fComInit)
            {
                hr = pFileDlg.CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER);
            }

            if (FAILED(hr))
            {
                break;
            }

            hr = pFileDlg->SetOptions(fos);
            if (FAILED(hr))
            {
                break;
            }

            UINT filter_size = filter.GetSize();
            UINT filter_index = filter.GetFileTypeIndex();

            if (filter_size)
            {
                pFileDlg->SetFileTypes(filter_size, filter);
            }

            if (0 != filter_index)
            {
                pFileDlg->SetFileTypeIndex(filter_index);
            }

            _tstring strFileDir = CPathUtils::GetFileDir(strPath);
            _tstring strFileName = CPathUtils::GetFileName(strPath, true);
            if (strFileDir.empty())
            {
                strFileDir = CPathUtils::GetCurrentModuleDir();
            }
            else
            {
                if (CPathUtils::IsFile(strFileDir))
                {
                    strFileDir = CPathUtils::GetFileDir(strFileDir);
                }
            }

            if (CPathUtils::IsDirectory(strFileDir))
            {
                CComPtr<IShellItem> pItem;
                if (SUCCEEDED(::SHCreateItemFromParsingName(CStrUtils::TStrToWStr(strFileDir).c_str(), nullptr, IID_PPV_ARGS(&pItem))))
                {
                    pFileDlg->SetFolder(pItem);
                    pFileDlg->SetFileName(CStrUtils::TStrToWStr(strFileName).c_str());
                }
            }

            hr = pFileDlg->Show(hOwner);
            if (SUCCEEDED(hr))
            {
                UINT nTypeIndex = 0;
                pFileDlg->GetFileTypeIndex(&nTypeIndex);
                filter.SetFileTypeIndex(nTypeIndex);

                CComPtr<IShellItem> pItem;
                if (SUCCEEDED(pFileDlg->GetResult(&pItem)))
                {
                    LPWSTR pszName = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
                    {
                        strPath = CStrUtils::WStrToTStr(pszName);
                        ::CoTaskMemFree(pszName);
                    }
                }
            }
        } while (FALSE);

        if (fComInit)
        {
            ::CoUninitialize();
        }

        return SUCCEEDED(hr);
    }

    bool GetSavePath(HWND hOwner, _tstring& strPath, CDlgFilterSpec& filter)
    {
        HRESULT hr = S_OK;
        bool fComInit = false;

        do
        {
            CComPtr<IFileSaveDialog> pFileDlg;
            FILEOPENDIALOGOPTIONS fos = FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES | FOS_DONTADDTORECENT | FOS_NOCHANGEDIR;

            hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                break;
            }

            fComInit = SUCCEEDED(hr);
            if (fComInit)
            {
                hr = pFileDlg.CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER);
            }

            if (FAILED(hr))
            {
                break;
            }

            hr = pFileDlg->SetOptions(fos);
            if (FAILED(hr))
            {
                break;
            }

            UINT filter_size = filter.GetSize();
            UINT filter_index = filter.GetFileTypeIndex();

            if (filter_size)
            {
                pFileDlg->SetFileTypes(filter_size, filter);
            }

            if (0 != filter_index)
            {
                pFileDlg->SetFileTypeIndex(filter_index);
            }

            _tstring strFileDir = strPath;
            _tstring strFileName;
            if (strFileDir.empty())
            {
                strFileDir = CPathUtils::GetCurrentModuleDir();
            }
            else
            {
                strFileDir = CPathUtils::GetFileDir(strFileDir);
            }

            strFileName = CPathUtils::GetFileName(strPath, true);

            if (!strFileDir.empty() && CPathUtils::IsExist(strFileDir))
            {
                CComPtr<IShellItem> pItem;
                hr = ::SHCreateItemFromParsingName(CStrUtils::TStrToWStr(strFileDir).c_str(), nullptr, IID_PPV_ARGS(&pItem));
                if (SUCCEEDED(hr))
                {
                    pFileDlg->SetFolder(pItem);
                    pFileDlg->SetFileName(CStrUtils::TStrToWStr(strFileName).c_str());
                }
            }

            auto DlgEventCb = [&filter](__RPC__in_opt IFileDialog* pfd) {
                UINT nTypeIndex = 0;
                pfd->GetFileTypeIndex(&nTypeIndex);
                filter.SetFileTypeIndex(nTypeIndex);

                LPWSTR pszName = nullptr;
                if (SUCCEEDED(pfd->GetFileName(&pszName)) && nullptr != pszName)
                {
                    _tstring strFileName = CStrUtils::WStrToTStr(pszName);
                    _tstring strName = CPathUtils::GetFileName(strFileName, false);
                    _tstring strExt = CPathUtils::GetFileExt(strFileName, true);

                    if (!strFileName.empty())
                    {
                        FILTER_SPEC specs = filter.GetFilterSpecInfo(nTypeIndex);
                        bool find_flag = false;
                        for (const auto& item : specs.vSpecList)
                        {
                            if (strExt == item)
                            {
                                find_flag = true;
                                break;
                            }
                        }

                        if (!find_flag)
                        {
                            for (const auto& item : specs.vSpecList)
                            {
                                if (item != _T(".*"))
                                {
                                    strFileName = strName + item;
                                    pfd->SetFileName(CStrUtils::TStrToWStr(strFileName).c_str());
                                    break;
                                }
                            }
                        }
                    }

                    ::CoTaskMemFree(pszName);
                }

                return S_OK;
                };

            CComPtr<FileDialogEvents> pEvent;
            FileDialogEvents::CreateInstance(&pEvent);

            DWORD dwCookie = 0;
            bool fAdviseEvent = false;

            if (pEvent)
            {
                pEvent->SetOnTypeChange(DlgEventCb);
                fAdviseEvent = SUCCEEDED(pFileDlg->Advise(pEvent, &dwCookie));
            }

            hr = pFileDlg->Show(hOwner);
            if (SUCCEEDED(hr))
            {
                CComPtr<IShellItem> pItem;
                if (SUCCEEDED(pFileDlg->GetResult(&pItem)))
                {
                    LPWSTR pszName = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
                    {
                        strPath = CStrUtils::WStrToTStr(pszName);
                        ::CoTaskMemFree(pszName);
                    }
                }
            }

            if (pEvent)
            {
                if (fAdviseEvent)
                {
                    pFileDlg->Unadvise(dwCookie);
                }
            }

        } while (false);

        if(HRESULT_FROM_WIN32(ERROR_CANCELLED) != hr)
        {
            _tstring strDir = CPathUtils::GetFileDir(strPath);
            _tstring strName = CPathUtils::GetFileName(strPath, false);
            _tstring strExt = CPathUtils::GetFileExt(strPath, true);

            if (!strPath.empty())
            {
                FILTER_SPEC specs = filter.GetFilterSpecInfo(filter.GetFileTypeIndex());
                bool find_flag = false;
                for (const auto& item : specs.vSpecList)
                {
                    if (strExt == item)
                    {
                        find_flag = true;
                        break;
                    }
                }

                if (!find_flag)
                {
                    for (const auto& item : specs.vSpecList)
                    {
                        if (item != _T(".*"))
                        {
                            strPath = strDir + _T("\\") + strName + item;
                            break;
                        }
                    }
                }
            }
        }

        if (fComInit)
        {
            ::CoUninitialize();
        }

        return SUCCEEDED(hr);
    }

    bool GetDirPath(HWND hOwner, _tstring& strDir)
    {
        HRESULT hr = S_OK;
        bool fComInit = false;

        do
        {
            CComPtr<IFileOpenDialog> pFileDlg;
            FILEOPENDIALOGOPTIONS fos = FOS_PICKFOLDERS | FOS_NOCHANGEDIR;

            hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                break;
            }

            fComInit = SUCCEEDED(hr);
            if (fComInit)
            {
                hr = pFileDlg.CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER);
            }

            if (FAILED(hr))
            {
                break;
            }

            hr = pFileDlg->SetOptions(fos);
            if (FAILED(hr))
            {
                break;
            }

            _tstring strFileDir = strDir;
            if (strFileDir.empty())
            {
                strFileDir = CPathUtils::GetCurrentModuleDir();
            }
            else
            {
                if (CPathUtils::IsDirectory(strFileDir))
                {
                    CComPtr<IShellItem> pItem;
                    hr = ::SHCreateItemFromParsingName(CStrUtils::TStrToWStr(strFileDir).c_str(), nullptr, IID_PPV_ARGS(&pItem));
                    if (SUCCEEDED(hr))
                    {
                        pFileDlg->SetFolder(pItem);
                    }
                }
            }

            hr = pFileDlg->Show(hOwner);
            if (SUCCEEDED(hr))
            {
                CComPtr<IShellItem> pItem;
                if (SUCCEEDED(pFileDlg->GetResult(&pItem)))
                {
                    LPWSTR pszName = nullptr;
                    if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
                    {
                        strDir = CStrUtils::WStrToTStr(pszName);
                        ::CoTaskMemFree(pszName);
                    }
                }
            }

        } while (FALSE);

        if (fComInit)
        {
            ::CoUninitialize();
        }

        return SUCCEEDED(hr);
    }

    std::vector<_tstring> GetFilePathList(HWND hOwner, _tstring& strDir, CDlgFilterSpec& filter)
    {
        std::vector<_tstring> vFileList;
        HRESULT hr = S_OK;
        bool fComInit = false;

        do
        {
            CComPtr<IFileOpenDialog> pFileDlg;
            FILEOPENDIALOGOPTIONS fos = FOS_ALLOWMULTISELECT | FOS_FILEMUSTEXIST;

            hr = ::CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr))
            {
                break;
            }

            fComInit = SUCCEEDED(hr);
            if (fComInit)
            {
                hr = pFileDlg.CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER);
            }

            if (FAILED(hr))
            {
                break;
            }

            hr = pFileDlg->SetOptions(fos);
            if (FAILED(hr))
            {
                break;
            }

            UINT filter_size = filter.GetSize();
            UINT filter_index = filter.GetFileTypeIndex();

            if (filter_size)
            {
                pFileDlg->SetFileTypes(filter_size, filter);
            }

            if (0 != filter_index)
            {
                pFileDlg->SetFileTypeIndex(filter_index);
            }

            _tstring strFileDir = strDir;
            if (strFileDir.empty())
            {
                strFileDir = CPathUtils::GetCurrentModuleDir();
            }

            if (CPathUtils::IsFile(strFileDir))
            {
                strFileDir = CPathUtils::GetFileDir(strFileDir);
            }

            if (CPathUtils::IsDirectory(strFileDir))
            {
                CComPtr<IShellItem> pItem;
                if (SUCCEEDED(::SHCreateItemFromParsingName(CStrUtils::TStrToWStr(strFileDir).c_str(), nullptr, IID_PPV_ARGS(&pItem))))
                {
                    pFileDlg->SetFolder(pItem);
                }
            }

            hr = pFileDlg->Show(hOwner);
            if (SUCCEEDED(hr))
            {
                CComPtr<IShellItemArray> pItemArray;
                hr = pFileDlg->GetResults(&pItemArray);

                DWORD dwCount = 0;
                pItemArray->GetCount(&dwCount);

                for (DWORD i = 0; i < dwCount; i++)
                {
                    CComPtr<IShellItem> pItem;
                    pItemArray->GetItemAt(i, &pItem);
                    if (pItem)
                    {
                        LPWSTR pszName = nullptr;
                        if (SUCCEEDED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszName)))
                        {
                            if (0 == i)
                            {
                                strDir = CPathUtils::GetFileDir(CStrUtils::WStrToTStr(pszName));
                            }

                            vFileList.push_back(CStrUtils::WStrToTStr(pszName));
                            ::CoTaskMemFree(pszName);
                        }
                    }
                }
            }

        } while (FALSE);

        if (fComInit)
        {
            ::CoUninitialize();
        }

        return vFileList;
    }
}