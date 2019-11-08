#include "stdinc.hpp"
#include "Dialog.hpp"
#include <boost/format.hpp>

Dialog::Dialog(void)
{
	hBackground = CreateSolidBrush(RGB(255, 255, 255));

	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	hFont = CreateFontIndirectW(&ncm.lfMessageFont);
}

Dialog::~Dialog(void)
{
	DeleteObject(hBackground);
	DeleteObject(hFont);
}

LRESULT Dialog::onCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetChildFont(m_hWnd, hFont);

	CMessageLoop* pLoop = _Module.GetMessageLoop();
    pLoop->AddMessageFilter(this);

	DlgResize_Init(true, true, WS_THICKFRAME);

	{
		std::wstring ret;
		ret.resize(1024);
		ret.resize(::GetModuleFileNameW(::GetModuleHandle(NULL), &ret[0], ret.size()-1));

		std::wstring::size_type i = ret.rfind(L"\\");
		
		if(i != std::wstring::npos)
		{
			ret = ret.substr(0, i).c_str();
		}

		currentDir = ret;
	}

	SetDlgItemText(IDC_ROOT_PATH, currentDir.c_str());

	CRect rc;
	ctrlList.Attach(GetDlgItem(IDC_LIST));
	ctrlList.GetClientRect(&rc);
	rc.right -= GetSystemMetrics(SM_CXVSCROLL) + 2;

	ctrlList.InsertColumn(0, L"Old name", LVCFMT_LEFT, rc.Width() / 2, 0);
	ctrlList.InsertColumn(1, L"New name", LVCFMT_LEFT, rc.Width() / 2, 0);
	ctrlList.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT);

	SHFILEINFO stInfo; 
	imageList.Attach((HIMAGELIST)SHGetFileInfo(L"c:\\", 0, &stInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
	ctrlList.SetImageList(imageList, LVSIL_SMALL);

	if(RunTimeHelper::IsVista() && RunTimeHelper::IsThemeAvailable())
	{
		typedef HRESULT (CALLBACK* LPFUNC)(HWND, LPCWSTR, LPCWSTR);
		HMODULE uxdll = LoadLibrary(_T("uxtheme.dll"));
		
		if(uxdll)
		{
			LPFUNC _SetWindowTheme = (LPFUNC)GetProcAddress(uxdll, "SetWindowTheme");
			
			if(_SetWindowTheme)
			{
				_SetWindowTheme(ctrlList.m_hWnd, L"explorer", 0);
			}

			FreeLibrary(uxdll);
		}
	}
	
	return 0;
}

BOOL Dialog::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

LRESULT Dialog::onDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	join(0);

	for(int i = 0; i < ctrlList.GetItemCount(); ++i)
	{
		((item*)ctrlList.GetItemData(i))->dec();
	}

	ctrlList.DeleteAllItems();
	params.items.clear();

	ctrlList.Detach();
	imageList.Detach();
	return 0;
}

LRESULT Dialog::onClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	DestroyWindow();
	PostQuitMessage(0);
	return 0;
}

LRESULT Dialog::onColor(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	return (LRESULT)hBackground;
}

LRESULT Dialog::OnBnClickedBrowseRootPath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFolderDialog dlg(m_hWnd);
	std::wstring root = getText(IDC_ROOT_PATH);

	dlg.SetInitialFolder(root.c_str());

	if(dlg.DoModal(m_hWnd) == IDOK)
	{
		SetDlgItemText(IDC_ROOT_PATH, dlg.m_szFolderPath);
	}

	return 0;
}

LRESULT Dialog::onAddItem(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	item_ptr p = reinterpret_cast<item*>(wParam);

	int pos = addItem(p->oldName.c_str(), p->newName.c_str(), p->iconId, p->newName.empty() ? 0 : 1);
	ctrlList.SetItemData(pos, (DWORD_PTR)p.get());
	return 0;
}

LRESULT Dialog::onUpdateStatus(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	std::wstring* str = reinterpret_cast<std::wstring*>(wParam);

	if(str == 0 || str->length() <= 0)
	{
		SetDlgItemText(IDC_LOG, L"Idle");
	}
	else
	{
		SetDlgItemText(IDC_LOG, str->c_str());
	}

	delete str;
	return 0;
}

LRESULT Dialog::onLockControls(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CWindow search = GetDlgItem(IDC_SEARCH);
	CWindow rename = GetDlgItem(IDC_RENAME_FILES);

	search.EnableWindow(wParam != 0 ? FALSE : TRUE);
	rename.EnableWindow(wParam != 0 ? FALSE : TRUE);

	return 0;
}

LRESULT Dialog::OnBnClickedSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(isThreadRunning() == true)
	{
		join(0);
	}

	for(int i = 0; i < ctrlList.GetItemCount(); ++i)
	{
		((item*)ctrlList.GetItemData(i))->dec();
	}

	ctrlList.DeleteAllItems();

	PostMessage(WM_LOCK_CONTROLS, TRUE);

	{
		Lock l(cs);
		params.root = getText(IDC_ROOT_PATH);
		params.recursive = ::SendMessage(GetDlgItem(IDC_RECURSIVE), BM_GETCHECK, 0, 0L) != 0 ? true : false;

		params.pattern = getText(IDC_PATTERN);
		params.format = getText(IDC_FORMAT);
		params.icase = ::SendMessage(GetDlgItem(IDC_CASE_SENSITIVE), BM_GETCHECK, 0, 0L) != 0 ? false : true;

		params.items.clear();

		threadMethod = std::bind(&Dialog::thread_Search, this, std::placeholders::_1);
	}

	runThread();

	return 0;
}

LRESULT Dialog::OnBnClickedRenameFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(isThreadRunning() == true)
	{
		join();
	}

	PostMessage(WM_LOCK_CONTROLS, TRUE);

	{
		Lock l(cs);
		params.items.clear();

		for(int i = 0; i < ctrlList.GetItemCount(); ++i)
		{
			item* it = (item*)ctrlList.GetItemData(i);

			if(it->isFile == true)
			{
				params.items.push_back(it);
			}
		}

		threadMethod = std::bind(&Dialog::thread_Rename, this, std::placeholders::_1);
	}

	runThread();

	return 0;
}

LRESULT Dialog::OnBnClickedDeleteFiles(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(isThreadRunning() == true)
	{
		join();
	}

	PostMessage(WM_LOCK_CONTROLS, TRUE);

	{
		Lock l(cs);
		params.items.clear();

		for(int i = 0; i < ctrlList.GetItemCount(); ++i)
		{
			item* it = (item*)ctrlList.GetItemData(i);

			if(it->isFile == true)
			{
				params.items.push_back(it);
			}
		}

		threadMethod = std::bind(&Dialog::thread_Delete, this, std::placeholders::_1);
	}

	runThread();

	return 0;
}

void Dialog::thread_main()
{
	std::function<void(ThreadParams&)> func;
	ThreadParams params;

	{
		Lock l(cs);
		params = this->params;
		func = threadMethod;
	}

	if(func == nullptr)
	{
		return;
	}

	try
	{
		func(params);
	}
	catch(...)
	{
	}

	PostMessage(WM_LOCK_CONTROLS, FALSE);
}

void Dialog::thread_Search(ThreadParams& params)
{
	WIN32_FIND_DATA data;
	HANDLE hFind;

	if(params.format.empty())
	{
		params.format = L"$0";
	}

	if(params.pattern.empty())
	{
		params.pattern = L".*";
	}

	std::wregex rx;

	try
	{
		rx = std::wregex(params.pattern, params.icase ? std::regex_constants::ECMAScript | std::regex_constants::icase : std::regex_constants::ECMAScript);
	}
	catch(const std::regex_error& err)
	{
		MessageBoxA(m_hWnd, err.what(), "Regular Expression error", MB_ICONERROR);
		return;
	}

	std::vector<item*> items;
	std::queue<std::wstring> dirs;
	dirs.push(params.root + L"\\");

	int count = 0;
	int countAll = 0;

	while(dirs.empty() == false)
	{
		updateStatus(L"Processing: " + dirs.front());

		hFind = FindFirstFile((dirs.front() + L"*").c_str(), &data);

		if(hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if((data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) || (data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				{
					continue;
				}

				std::wstring fullPath = dirs.front() + data.cFileName;

				if((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					if(wcscmp(data.cFileName, L"..") == 0 || wcscmp(data.cFileName, L".") == 0)
					{
						continue;
					}

					if(params.recursive)
					{
						dirs.push(fullPath + L"\\");
					}

					debug(L"folder: %s\n", dirs.back().c_str());
				}
				else
				{
					++countAll;

					try
					{
						std::wstring oldName = data.cFileName;

						if(std::regex_search(oldName, rx))
						{
							std::wstring newName = std::regex_replace(oldName, rx, params.format);

							item* i = new item(dirs.front(), std::move(oldName), std::move(newName));
							i->inc();
							i->iconId = getIconId((i->root + i->oldName).c_str());
							i->isFile = true;

							items.push_back(i);
							++count;
						}
					}
					catch(...)
					{
					}
				}
			}
			while(FindNextFile(hFind, &data) != 0);

			FindClose(hFind);
		}

		if(items.size() > 0)
		{
			{
				item* i = new item(dirs.front(), dirs.front());
				i->inc();
				i->iconId = getIconId(i->root.c_str());

				items.insert(items.begin(), i);
			}

			for(auto i = items.begin(); i != items.end(); ++i)
			{
				PostMessage(WM_ADD_ITEM, (WPARAM)(*i));
			}
		}

		items.clear();
		dirs.pop();
	}

	updateStatus(boost::str(boost::wformat(L"Matched %1% out of %2% file(s)") % count % countAll));
}

void Dialog::thread_Rename(ThreadParams& params)
{
	int count = 0;
	std::for_each(params.items.begin(), params.items.end(), [&](item_ptr& i)
	{
		if(i->rename())
		{
			updateStatus(i->oldName + L" => " + i->newName);
			++count;
		}
	});

	updateStatus(boost::str(boost::wformat(L"Renamed %1%/%2% file(s)") % count % params.items.size()));
	params.items.clear();
}

void Dialog::thread_Delete(ThreadParams& params)
{
	int count = 0;
	std::for_each(params.items.begin(), params.items.end(), [&](item_ptr& i)
	{
		if(i->deleteFile())
		{
			updateStatus(i->oldName + L" => " + i->newName);
			++count;
		}
	});

	updateStatus(boost::str(boost::wformat(L"Deleted %1%/%2% file(s)") % count % params.items.size()));
	params.items.clear();
}

std::wstring Dialog::getText(int id) const
{
	std::wstring ret;
	CWindow wnd = GetDlgItem(id);

	ret.resize(wnd.GetWindowTextLength()+1);
	ret.resize(wnd.GetWindowText(&ret[0], ret.size()));

	wnd.Detach();
	
	return std::move(ret);
}

int Dialog::getIconId(LPCWSTR path) const
{
	SHFILEINFO stInfo;
	SHGetFileInfoW(path, 0, &stInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
	int ret = stInfo.iIcon;
	return ret;
}

int Dialog::addItem(LPCWSTR oldPath, LPCWSTR newPath, int iconId /*= -1*/, int indent /*= 0*/, int pos /*= -1*/)
{
	if(pos < 0)
	{
		pos = ctrlList.GetItemCount();
	}

	LVITEM lvItem = { 0 };
	lvItem.mask = LVIF_TEXT;
	lvItem.iItem = pos;
	lvItem.iSubItem = 0;
	lvItem.pszText = (LPTSTR)oldPath;

	if(iconId != -1)
	{
		lvItem.mask |= LVIF_IMAGE;
		lvItem.iImage = iconId;
	}

	if(indent > 0)
	{
		lvItem.mask |= LVIF_INDENT;
		lvItem.iIndent = indent;
	}

	pos = ctrlList.InsertItem(&lvItem);
	ctrlList.SetItemText(pos, 1, newPath);

	return pos;
}

void Dialog::updateStatus(LPCWSTR str)
{
	std::wstring* cpy = new std::wstring(str != 0 ? str : L"Idle");
	PostMessage(WM_UPDATE_STATUS, (WPARAM)cpy);
}
