/*
 * Copyright (C) 2012 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#pragma once

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#define STRICT

#define _WTL_NO_CSTRING

#define _ATL_NO_OPENGL
#define _ATL_NO_COM
#define _ATL_NO_HOSTING
#define _ATL_NO_OLD_NAMES
#define _ATL_NO_COM_SUPPORT
#define _ATL_NO_PERF_SUPPORT
#define _ATL_NO_SERVICE
#define _ATL_NO_DOCHOSTUIHANDLER

#define WINVER           0x0600
#define _WIN32_WINNT     0x0600
#define _WIN32_IE        0x0700
#define _RICHEDIT_VER    0x0300

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
