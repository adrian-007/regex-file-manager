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

#include "resource.h"
#include "Thread.hpp"
#include "item.hpp"

#define WM_ADD_ITEM WM_USER+1
#define WM_UPDATE_STATUS WM_USER+2
#define WM_LOCK_CONTROLS WM_USER+3

class Dialog : public CDialogImpl<Dialog>, public CDialogResize<Dialog>, public CMessageFilter, private Thread
{
public:
	enum { IDD = IDD_DIALOG };

	Dialog(void);
	~Dialog(void);

	BEGIN_DLGRESIZE_MAP(Dialog)
		DLGRESIZE_CONTROL(IDC_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_SEARCH_OPTIONS_GROUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_REGEX_OPTIONS_GROUP, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_ROOT_PATH, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BROWSE_ROOT_PATH, DLSZ_MOVE_X)
		DLGRESIZE_CONTROL(IDC_PATTERN, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_FORMAT, DLSZ_SIZE_X)

		DLGRESIZE_CONTROL(IDC_LOG, DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_LOG, DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_SEARCH, DLSZ_MOVE_Y)
 		DLGRESIZE_CONTROL(IDC_RENAME_FILES, DLSZ_MOVE_Y)
  		DLGRESIZE_CONTROL(IDC_DELETE_FILES, DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(Dialog)
		MESSAGE_HANDLER(WM_INITDIALOG, onCreate)
		MESSAGE_HANDLER(WM_DESTROY, onDestroy)
		MESSAGE_HANDLER(WM_CLOSE, onClose)
		MESSAGE_HANDLER(WM_CTLCOLORDLG, onColor)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC, onColor)
		// custom messages to make thread-safe calls to gui
		MESSAGE_HANDLER(WM_ADD_ITEM, onAddItem)
		MESSAGE_HANDLER(WM_UPDATE_STATUS, onUpdateStatus)
		MESSAGE_HANDLER(WM_LOCK_CONTROLS, onLockControls)
		//
		COMMAND_HANDLER(IDC_BROWSE_ROOT_PATH, BN_CLICKED, OnBnClickedBrowseRootPath)
		COMMAND_HANDLER(IDC_SEARCH, BN_CLICKED, OnBnClickedSearch)
		COMMAND_HANDLER(IDC_RENAME_FILES, BN_CLICKED, OnBnClickedRenameFiles)
		CHAIN_MSG_MAP(CDialogResize<Dialog>)
		COMMAND_HANDLER(IDC_DELETE_FILES, BN_CLICKED, OnBnClickedDeleteFiles)
	END_MSG_MAP()

private:
	struct ThreadParams
	{
		std::wstring root;
		bool recursive;

		std::wstring pattern;
		std::wstring format;
		bool icase;

		std::vector<item_ptr> items;
	};

	BOOL PreTranslateMessage(MSG* pMsg);

	void thread_main();

	void thread_Search(ThreadParams& params);
	void thread_Rename(ThreadParams& params);
	void thread_Delete(ThreadParams& params);

	std::wstring getText(int id) const;
	int getIconId(LPCWSTR path) const;
	int addItem(LPCWSTR oldPath, LPCWSTR newPath, int iconId = -1, int indent = 0, int pos = -1);

	void updateStatus(LPCWSTR str);

	inline void updateStatus(const std::wstring& str)
	{
		updateStatus(str.c_str());
	}

	LRESULT onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onAddItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onUpdateStatus(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT onLockControls(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedBrowseRootPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedRenameFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBnClickedDeleteFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	HBRUSH hBackground;
	HFONT hFont;
	CImageList imageList;

	std::wstring currentDir;

	ThreadParams params;
	std::function<void(ThreadParams& params)> threadMethod;

	CListViewCtrl ctrlList;
	CriticalSection cs;
};

class SetChildFont
{
public:
	SetChildFont(HWND parent, HFONT font)
	{
		::SendMessage(parent, WM_SETFONT, (WPARAM)font, 0);
		HWND child = ::GetWindow(parent, GW_CHILD);
		
		while(child)
		{
			::SendMessage(child, WM_SETFONT, (WPARAM)font, 0);
			child = ::GetNextWindow(child, GW_HWNDNEXT);
		}
	}
};
