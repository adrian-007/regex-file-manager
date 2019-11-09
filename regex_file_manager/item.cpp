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

#include "stdinc.hpp"
#include "item.hpp"
#include "Thread.hpp"

item::item(const std::wstring& _root, const std::wstring& _oldName, const std::wstring& _newName /*= L""*/) : __refCount(0), iconId(-1), root(_root), oldName(_oldName), newName(_newName), isFile(false)
{
	debug(L"[ctor] %p (%s)\n", this, oldName.c_str());
}

item::~item(void)
{
	debug(L"[dtor] %p (%s)\n", this, oldName.c_str());
}

bool item::rename()
{
	if(isFile == false)
	{
		return false;
	}

	std::wstring oldPath = root + oldName;
	std::wstring newPath = root + newName;

	bool ret = ::MoveFileW(oldPath.c_str(), newPath.c_str()) != 0 ? true : false;

	debug(L"%s => %s [%s]\n", oldPath.c_str(), newPath.c_str(), (ret ? L"OK" : L"ERROR"));

	return ret;
}

bool item::deleteFile()
{
	if(isFile == false)
	{
		return false;
	}

	std::wstring path = root + oldName;

	bool ret = ::DeleteFileW(path.c_str()) != 0 ? true : false;

	debug(L"%s => delete [%s]\n", path.c_str(), (ret ? L"OK" : L"ERROR"));

	return ret;
}

void intrusive_ptr_add_ref(item* p)
{
	Thread::AtomicIncrement(&p->__refCount);
}

void intrusive_ptr_release(item* p)
{
	if(Thread::AtomicDecrement(&p->__refCount) <= 0)
	{
		delete p;
	}
}
