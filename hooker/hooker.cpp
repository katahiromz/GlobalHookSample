// hooker.cpp
// See README.txt and LICENSE.txt.
//////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS
#include "targetver.h"
#include "MWindowBase.hpp"
#include "payload.hpp"
#include "resource.h"
#include <cstdio>

//////////////////////////////////////////////////////////////////////////////
// MGlobalHookSample --- an instance of the application and the main window

class MGlobalHookSample : public MWindowBase
{
public:
    HINSTANCE m_hInst;

    MGlobalHookSample(HINSTANCE hInst) : m_hInst(hInst)
    {
    }

    virtual ~MGlobalHookSample()
    {
    }

    virtual LPCTSTR GetWndClassNameDx() const
    {
        return TEXT("MZC4 MGlobalHookSample Class");
    }

    virtual VOID ModifyWndClassDx(WNDCLASSEX& wcx)
    {
        wcx.hIcon = LoadIconDx(1);
        wcx.hIconSm = LoadSmallIconDx(1);
        wcx.lpszMenuName = MAKEINTRESOURCE(1);
    }

    BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
    {
        return TRUE;
    }

    void OnDestroy(HWND hwnd)
    {
        PostQuitMessage(0);
    }

    void OnHook(HWND hwnd)
    {
        TCHAR szPath[MAX_PATH];

        GetModuleFileName(NULL, szPath, MAX_PATH);
        TCHAR *pch = _tcsrchr(szPath, TEXT('\\'));
        lstrcpy(pch, TEXT("\\GlobalHookSample.txt"));

        HRESULT hres = hook(hwnd, szPath);
        if (SUCCEEDED(hres))
        {
            MsgBoxDx(TEXT("OK Hooked!"), MB_ICONINFORMATION);
        }
        else
        {
            ErrorBoxDx(TEXT("Failed to hook!"));
        }
    }

    void OnUnhook(HWND hwnd)
    {
        HRESULT hres = unhook(hwnd);
        if (SUCCEEDED(hres))
        {
            MsgBoxDx(TEXT("OK, unhooked!"), MB_ICONINFORMATION);
        }
        else
        {
            ErrorBoxDx(TEXT("Failed to unhook!"));
        }
    }

    void OnShowPath(HWND hwnd)
    {
        TCHAR szPath[MAX_PATH];
        if (getOutputFile(szPath, MAX_PATH))
        {
            MsgBoxDx(szPath, MB_ICONINFORMATION);
        }
        else
        {
            ErrorBoxDx(TEXT("failed"));
        }
    }

    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
        case CMDID_EXIT:
            PostMessageDx(WM_CLOSE);
            break;
        case CMDID_HOOK:
            OnHook(hwnd);
            break;
        case CMDID_UNHOOK:
            OnUnhook(hwnd);
            break;
        case CMDID_SHOWPATH:
            OnShowPath(hwnd);
            break;
        }
    }

    virtual LRESULT CALLBACK
    WindowProcDx(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
        default:
            return DefaultProcDx();
        }
    }

    INT StartDx(INT nCmdShow)
    {
        DWORD style = WS_OVERLAPPEDWINDOW;
        DWORD exstyle = 0;
        if (!CreateWindowDx(NULL, TEXT("GlobalHookSample"), style, exstyle,
                            CW_USEDEFAULT, CW_USEDEFAULT, 300, 300))
        {
            ErrorBoxDx(TEXT("CreateWindow failed!"));
            return -1;
        }

        ShowWindow(m_hwnd, nCmdShow);
        UpdateWindow(m_hwnd);
        return 0;
    }

    INT RunDx()
    {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return (INT)msg.wParam;
    }
};

//////////////////////////////////////////////////////////////////////////////
// the Windows application main function

extern "C" INT WINAPI
_tWinMain(HINSTANCE   hInstance,
          HINSTANCE   hPrevInstance,
          LPTSTR      lpCmdLine,
          INT         nCmdShow)
{
    INT ret = -1;

    {
        MGlobalHookSample app(hInstance);
        ret = app.StartDx(nCmdShow);
        if (ret == 0)
        {
            ret = app.RunDx();
        }
    }

#if (WINVER >= 0x0500)
    HANDLE hProcess = GetCurrentProcess();
    DebugPrintDx(TEXT("Count of GDI objects: %ld\n"),
                 GetGuiResources(hProcess, GR_GDIOBJECTS));
    DebugPrintDx(TEXT("Count of USER objects: %ld\n"),
                 GetGuiResources(hProcess, GR_USEROBJECTS));
#endif

#if defined(_MSC_VER) && !defined(NDEBUG)
    // for detecting memory leak (MSVC only)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return ret;
}

//////////////////////////////////////////////////////////////////////////////
