#include "stdinc.hpp"
#include "Dialog.hpp"

CAppModule _Module;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	AtlInitCommonControls(ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES);

	CMessageLoop loop;
	_Module.Init(0, hInstance);
	_Module.AddMessageLoop(&loop);

	int ret = -1;

	{
		Dialog dlg;

		dlg.Create(0);
		dlg.ShowWindow(SW_SHOW);

		ret = loop.Run();

		_Module.RemoveMessageLoop();
	}

	_Module.Term();

	return ret;
}

std::wstring toWide(const std::string& str)
{
	std::wstring ret;
	
	if(str.empty() == false)
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, 0, 0);
		
		if(size > 0)
		{
			ret.resize(size);
			size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.size(), &ret[0], (int)ret.size());
			
			if(size == 0)
			{
				ret.clear();
			}
			else
			{
				ret.resize(size);
			}
		}
	}

	return ret;
}

std::string fromWide(const std::wstring& str) 
{
	std::string ret;

	if(str.empty() == false)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, 0, 0, 0, 0);
		
		if(size > 0)
		{
			ret.resize(size);
			size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), str.size(), &ret[0], (int)ret.size(), 0, 0);
			
			if(size == 0)
			{
				ret.clear();
			}
			else
			{
				ret.resize(size);
			}
		}
	}
	return ret;
}

#define DEBUG_BUFFER_SIZE 1024*4

void debug(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char* buf = new char[DEBUG_BUFFER_SIZE];
	_vsnprintf(buf, DEBUG_BUFFER_SIZE, format, args);
	va_end(args);

	::OutputDebugStringA(buf);

	delete buf;
}

void debug(const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);

	wchar_t* buf = new wchar_t[DEBUG_BUFFER_SIZE];
	_vsnwprintf(buf, DEBUG_BUFFER_SIZE, format, args);
	va_end(args);

	::OutputDebugStringW(buf);

	delete buf;
}
