// MyDLL.cpp --- a DLL for global hook
// See README.txt and LICENSE.txt.
//////////////////////////////////////////////////////////////////////////////

#include "payload.hpp"
#include "MInterProcessShare.hpp"
#include <cstdio>
#include <tchar.h>

#ifndef _countof
    #define _countof(array)     sizeof(array) / sizeof(array[0])
#endif

typedef MInterProcessShare<PAYLOAD_SHARE> PayloadShare;
#define SHARE_NAME TEXT("GlobalHookSampleShare")

//////////////////////////////////////////////////////////////////////////////

PayloadShare share(SHARE_NAME);
HINSTANCE g_hinstDLL;

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// DLL functions

PAYLOADAPI BOOL setOutputFile(const TCHAR *pszFileName)
{
    if (PAYLOAD_SHARE *pShare = share.Lock())
    {
        lstrcpyn(pShare->m_szOutputFile, pszFileName,
                 _countof(pShare->m_szOutputFile));
        share.Unlock();
        return TRUE;
    }
    return FALSE;
}

PAYLOADAPI BOOL getOutputFile(LPTSTR pszFileName, INT cchFileName)
{
    if (PAYLOAD_SHARE *pShare = share.Lock())
    {
        lstrcpyn(pszFileName, pShare->m_szOutputFile, cchFileName);
        share.Unlock();
        return TRUE;
    }
    return FALSE;
}

PAYLOADAPI LRESULT CALLBACK
KeyboardProc(INT code, WPARAM wParam, LPARAM lParam)
{
    LRESULT nRet = 0;
    if (PAYLOAD_SHARE *pShare = share.Lock())
    {
        if (code < 0)
        {
            nRet = CallNextHookEx(pShare->m_hHook, code, wParam, lParam);
        }
        else
        {
            if (code == HC_ACTION)
            {
                if ((lParam & 0x80000000) == 0) // being pressed?
                {
                    if (lParam & 0x20000000)    // ALT is down?
                    {
                        ;
                    }
                    else if (wParam != 13)
                    {
                        using namespace std;
                        FILE *fp = _tfopen(pShare->m_szOutputFile, TEXT("a"));
                        if (fp)
                        {
                            fprintf(fp, "%d (0x%02X: %c)\n",
                                    (INT)wParam, (INT)wParam, (INT)wParam);
                            fclose(fp);
                        }
                    }
                }
            }
        }

        nRet = CallNextHookEx(pShare->m_hHook, code, wParam, lParam);

        share.Unlock();
    }
    return nRet;
}

PAYLOADAPI HRESULT hook(HWND hwnd, const TCHAR *pszOutputFile)
{
    HRESULT hres = E_FAIL;
    if (PAYLOAD_SHARE *pShare = share.Lock())
    {
        if (!pShare->m_hHook)
        {
            assert(pszOutputFile && pszOutputFile[0]);
            lstrcpyn(pShare->m_szOutputFile, pszOutputFile, MAX_PATH);

            pShare->m_hHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc,
                                               g_hinstDLL, 0);
            if (pShare->m_hHook)
            {
                hres = S_OK;
            }
        }
        share.Unlock();
    }
    return hres;
}

PAYLOADAPI HRESULT unhook(HWND hwnd)
{
    HRESULT hres = E_FAIL;
    if (PAYLOAD_SHARE *pShare = share.Lock())
    {
        if (pShare->m_hHook)
        {
            BOOL bRet = UnhookWindowsHookEx(pShare->m_hHook);
            pShare->m_hHook = NULL;
            if (bRet)
            {
                hres = S_OK;
            }
        }
        share.Unlock();
    }
    return hres;
}

// The DLL main function.
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugString(TEXT("DLL_PROCESS_ATTACH\n"));
        DisableThreadLibraryCalls(hinstDLL);
        g_hinstDLL = hinstDLL;
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugString(TEXT("DLL_PROCESS_DETACH\n"));
        break;
    case DLL_THREAD_ATTACH:
        OutputDebugString(TEXT("DLL_THREAD_ATTACH\n"));
        break;
    case DLL_THREAD_DETACH:
        OutputDebugString(TEXT("DLL_THREAD_DETACH\n"));
        break;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
} // extern "C"
#endif
