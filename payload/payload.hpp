// payload.hpp --- a payload DLL for global hook
// See README.txt and LICENSE.txt.
//////////////////////////////////////////////////////////////////////////////

#ifndef GLOBALHOOK_PAYLOAD_HPP
#define GLOBALHOOK_PAYLOAD_HPP

#include <windows.h>
#include <tchar.h>

//////////////////////////////////////////////////////////////////////////////
// switching DLL importing/exporting

#ifdef PAYLOAD_BUILD
    #define PAYLOADAPI    __declspec(dllexport)
#else
    #define PAYLOADAPI    __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PAYLOAD_SHARE
{
    HHOOK        m_hHook;
    TCHAR        m_szOutputFile[MAX_PATH];
} PAYLOAD_SHARE;

PAYLOADAPI BOOL setOutputFile(const TCHAR *pszFileName);
PAYLOADAPI BOOL getOutputFile(LPTSTR pszFileName, INT cchFileName);
PAYLOADAPI HRESULT hook(HWND hwnd, const TCHAR *pszOutputFile);
PAYLOADAPI HRESULT unhook(HWND hwnd);
PAYLOADAPI LRESULT CALLBACK KeyboardProc(INT code, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
} // extern "C"
#endif

//////////////////////////////////////////////////////////////////////////////

#endif  // ndef GLOBALHOOK_PAYLOAD_HPP
