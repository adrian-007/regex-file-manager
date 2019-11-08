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
