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

#include <boost/intrusive_ptr.hpp>

class item
{
public:
	item(const std::wstring& _root, const std::wstring& _oldName, const std::wstring& _newName = L"");
	virtual ~item(void);

	inline void inc()
	{
		intrusive_ptr_add_ref(this);
	}

	inline void dec()
	{
		intrusive_ptr_release(this);
	}

	std::wstring root;
	std::wstring oldName;
	std::wstring newName;

	int iconId;
	bool isFile;

	bool rename();
	bool deleteFile();

private:
	friend void intrusive_ptr_add_ref(item* p);
	friend void intrusive_ptr_release(item* p);

	volatile long __refCount;
};

typedef boost::intrusive_ptr<item> item_ptr;
