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

class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

	void lock();
	void unlock();

private:
	CRITICAL_SECTION cs;
};

class Semaphore
{
public:
	Semaphore();
	~Semaphore();

	void signal();
	bool wait();
	bool wait(int ms);

private:
	Semaphore(const Semaphore&);
	Semaphore& operator = (const Semaphore&);
	
	HANDLE hSemaphore;
};

class Lock
{
public:
	Lock(CriticalSection& _cs) : cs(_cs) 
	{
		cs.lock();
	}

	~Lock()
	{
		cs.unlock();
	}

private:
	Lock(const Lock&);
	Lock& operator=(const Lock&);
	CriticalSection& cs;
};

class Thread
{
public:
	Thread();

	bool runThread();
	void join(int timeout = -1);
	void sleep(int ms);
	bool isThreadRunning();

	virtual void thread_main() = 0;

	static inline long AtomicIncrement(volatile long* i)
	{
		return ::InterlockedIncrement(i);
	}

	static inline long AtomicDecrement(volatile long* i)
	{
		return ::InterlockedDecrement(i);
	}

	static inline long AtomicSet(volatile long* i, long value)
	{
		return ::InterlockedExchange(i, value);
	}

private:
	static unsigned int WINAPI threadProc(LPVOID param);
	HANDLE thread;

protected:
	DWORD threadId;
};
