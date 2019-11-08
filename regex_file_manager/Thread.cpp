#include "stdinc.hpp"
#include "Thread.hpp"
#include <process.h>

/** mutex impl */
CriticalSection::CriticalSection()
{
	InitializeCriticalSection(&cs);
}

CriticalSection::~CriticalSection()
{
	DeleteCriticalSection(&cs);
}

void CriticalSection::lock()
{
	EnterCriticalSection(&cs);
}

void CriticalSection::unlock()
{
	LeaveCriticalSection(&cs);
}

/** semaphore impl */
Semaphore::Semaphore()
{
	hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
}

Semaphore::~Semaphore()
{
	CloseHandle(hSemaphore);
}

void Semaphore::signal()
{ 
	ReleaseSemaphore(hSemaphore, 1, NULL); 
}

bool Semaphore::wait()
{
	return WaitForSingleObject(hSemaphore, INFINITE) == WAIT_OBJECT_0; 
}

bool Semaphore::wait(int ms)
{ 
	return WaitForSingleObject(hSemaphore, (DWORD)ms) == WAIT_OBJECT_0; 
}

/** thread impl */
Thread::Thread()
{
	thread = INVALID_HANDLE_VALUE;
}

bool Thread::runThread()
{
	join();
	thread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, &threadProc, this, 0, reinterpret_cast<unsigned int*>(&threadId)));

	if(thread == 0)
	{
		thread = INVALID_HANDLE_VALUE;
		return false;
	}
	return true;
}

void Thread::join(int timeout /*= -1*/)
{
	if(thread != INVALID_HANDLE_VALUE)
	{
		if(WaitForSingleObject(thread, timeout < 0 ? INFINITE : timeout) != WAIT_OBJECT_0)
		{
			::TerminateThread(thread, -1);
		}

		CloseHandle(thread);
		thread = INVALID_HANDLE_VALUE;
	}
}

void Thread::sleep(int ms)
{
	::Sleep(static_cast<DWORD>(ms));
}

bool Thread::isThreadRunning()
{
	return thread != INVALID_HANDLE_VALUE;
}

unsigned int WINAPI Thread::threadProc(LPVOID param)
{
	Thread* t = (Thread*)param;

	t->thread_main();
	t->thread = INVALID_HANDLE_VALUE;
	_endthreadex(0);

	return 0;
}
