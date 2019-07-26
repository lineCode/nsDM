/* -------------------------------------------------------------------------
//	FileName	£º	event.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015/05/06 ÐÇÆÚÈý 15:42:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_EVENT_H__
#define __BASE_EVENT_H__

#include <windows.h>
#include "base/non_copyable.h"
// -------------------------------------------------------------------------

namespace base {
	class Event: private NonCopyable
	{
	public:
		Event() 
			: hEvent_(NULL) {
		}

		~Event() {
			Destroy();
		}

	public:
		bool Create(LPCWSTR name, bool manual = false) {
			hEvent_ = ::CreateEvent(NULL, manual ? TRUE : FALSE, FALSE, name);
			return hEvent_ ? true : false;
		}

		bool Open(LPCWSTR name) {
			hEvent_ = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, name);
			return hEvent_ ? true : false;
		}

		bool Wait(unsigned long ms = INFINITE) {
			return ::WaitForSingleObject(hEvent_, ms) == WAIT_OBJECT_0;
		}

		bool Active() {
			return ::SetEvent(hEvent_) == TRUE;
		}

		bool Reset() {
			return ::ResetEvent(hEvent_) == TRUE;
		}

		void Destroy() {
			if (hEvent_) {
				CloseHandle(hEvent_);
				hEvent_ = NULL;
			}
		}

	public:
		HANDLE GetHandle() {
			return hEvent_;
		}

	protected:
		HANDLE  hEvent_;;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_EVENT_H__