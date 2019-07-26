/* -------------------------------------------------------------------------
//	FileName	£º	YPGLock.h
//	Creator		£º	(zc)
//	CreateTime	£º	2012-2-7 18:35:17
//	Description	£º	
//
// -----------------------------------------------------------------------*/
#ifndef __YPGLOCKER_H__
#define __YPGLOCKER_H__

#include "ypgdatareport_framework.h"

class YPGDATAREPORT_API YPGLock
{
public:
	YPGLock()
	{
		::InitializeCriticalSection(&m_sesion);
	}
	~YPGLock()
	{
		::DeleteCriticalSection(&m_sesion);
	}
public:
	void lock(void)
	{
		::EnterCriticalSection(&m_sesion);
	}
	void unlock(void)
	{
		::LeaveCriticalSection(&m_sesion);
	}
	BOOL tryLock()
	{
		return ::TryEnterCriticalSection(&m_sesion);
	}
private:
	CRITICAL_SECTION m_sesion;
};

class YPGDATAREPORT_API YPGLockIt
{
public:
	YPGLockIt(IN YPGLock* pLock)
		: m_pLock(pLock)
	{
		if (m_pLock)	{
			m_pLock->lock();
		}
	}
	~YPGLockIt()
	{
		if (m_pLock) {
			m_pLock->unlock();
		}
	}
private:
	YPGLock* m_pLock;
};

typedef YPGLock		YPGMutex;
typedef YPGLockIt    YPGMutexLock;

#endif	//__YPGLOCKER_H__