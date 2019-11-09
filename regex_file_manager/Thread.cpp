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
