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
