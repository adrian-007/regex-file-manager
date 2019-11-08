#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _WTL_NO_CSTRING

#define _ATL_NO_OPENGL
#define _ATL_NO_MSIMG
#define _ATL_NO_COM
#define _ATL_NO_HOSTING
#define _ATL_NO_OLD_NAMES
#define _ATL_NO_COM_SUPPORT
#define _ATL_NO_PERF_SUPPORT
#define _ATL_NO_SERVICE
#define _ATL_NO_DOCHOSTUIHANDLER

#define _WIN32_WINNT	0x0502
#define _WIN32_IE		0x0502

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <shellapi.h>

// ATL
#include <atlbase.h>
#include <atltypes.h>
// WTL
#include <atlapp.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlframe.h>

extern CAppModule _Module;

#endif // _WIN32

#include <string>
#include <deque>
#include <queue>
#include <algorithm>
#include <vector>

#include <regex>
#include <functional>

extern std::wstring toWide(const std::string& str);
extern std::string fromWide(const std::wstring& str);
extern void debug(const char* format, ...);
extern void debug(const wchar_t* format, ...);
