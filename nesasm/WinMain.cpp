#include <windows.h>
#include <tchar.h>
#include "CDialogFrame.h"
#include "resource.h"

int WINAPI _tWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR lpCmdLine,
    _In_ int nShowCmd)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    CDialogFrame dlg;
    return dlg.DoModalEx(IDD_DIALOG_FRAME, nullptr, false, true);
}