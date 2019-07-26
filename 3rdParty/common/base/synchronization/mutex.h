/* -------------------------------------------------------------------------
//	FileName	£º	mutex.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015/05/06 ÐÇÆÚÈý 15:42:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_MUTEX_H__
#define __BASE_MUTEX_H__

#include <windows.h>
#include "base/non_copyable.h"
// -------------------------------------------------------------------------

namespace base {
	class Mutex: private NonCopyable
	{
	public:
		Mutex() 
			: hMutex_(NULL) {
		}

		Mutex(LPCWSTR name) 
			: hMutex_(NULL) {
			Create(name);
		}

		~Mutex() {
			Destroy();
		}

	public:
		bool Create(LPCWSTR name) {
			hMutex_ = ::CreateMutex(NULL, FALSE, name);
			return hMutex_ ? true : false;
		}

		bool Open(LPCWSTR name) {
			hMutex_ = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);
			return hMutex_ ? true : false;
		}

		bool Lock(unsigned long ms = INFINITE) {
			return ::WaitForSingleObject(hMutex_, ms) == WAIT_OBJECT_0;
		}

		void Unlock() {
			::ReleaseMutex(hMutex_);
		}

		void Destroy() {
			if (hMutex_) {
				::CloseHandle(hMutex_);
				hMutex_ = NULL;
			}
		}
		
	public:
		HANDLE GetHandle() { 
			return hMutex_; 
		}

	protected:
		HANDLE  hMutex_;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_MUTEX_H__