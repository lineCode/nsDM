/* -------------------------------------------------------------------------
//	FileName	£º	thread.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015/05/06 ÐÇÆÚÈý 15:42:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#ifndef __THREAD_H__
#define __THREAD_H__

#include <windows.h>
#include <process.h>
#include "base/non_copyable.h"
// -------------------------------------------------------------------------

namespace base {

	template<typename ownerT>
	class Thread: private NonCopyable
	{
	public:
		enum ThreadState {
			emTSNormal = 0,
			emTSSuspend,
			emTSRunning,
			emTSDead,
		};

	public:
		Thread() 
			: hThread_(INVALID_HANDLE_VALUE)
			, needTerminate_(0)
			, state_(emTSNormal) 
			, owner_(NULL) {
		}

		virtual ~Thread() {
			Destroy();
		}

		void SetOwner(ownerT* owner) {
			owner_ = owner;
		}

	public:
		//typedef unsigned int(__stdcall* _THREAD_FUN)(void*);
		bool Create(bool createSuspended = true) {
			if (hThread_ != INVALID_HANDLE_VALUE) {
				return true;
			}
			InterlockedCompareExchange(&needTerminate_, 0, 1);
			unsigned int threadId = 0;
			hThread_ = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, (void*)this, createSuspended ? CREATE_SUSPENDED : 0, 
				(unsigned int*)&threadId);

			bool ret = (hThread_ != INVALID_HANDLE_VALUE);
			if (ret) {
				state_ = createSuspended ? emTSSuspend : emTSNormal;
			}
			return ret;
		}

		bool Suspend() {
			if (hThread_ == INVALID_HANDLE_VALUE) {
				return false;
			}
			unsigned long ret = SuspendThread(hThread_);
			if (ret != -1) {
				state_ = emTSSuspend;
			}
			return ret != -1;
		}

		bool Resume() {
			if (hThread_ == INVALID_HANDLE_VALUE) {
				return false;
			}
			unsigned long ret = ResumeThread(hThread_);
			if (ret != -1) {
				state_ = emTSRunning;
			}
			return ret != -1;
		}

		void Terminate() {
			InterlockedCompareExchange(&needTerminate_, 1, 0);
		}

		void ForceTerminate()
		{
			if (::WaitForSingleObject(hThread_, 0) == WAIT_TIMEOUT)
			{
				::TerminateThread(hThread_, -1);
			}
		}

		void Destroy() {
			state_ = emTSDead;
			if (hThread_ != INVALID_HANDLE_VALUE) {
				CloseHandle(hThread_);
				hThread_ = INVALID_HANDLE_VALUE;
			}
		}

	public:
		unsigned int WaitForEnd(unsigned long ms = INFINITE) {
			if (hThread_ == INVALID_HANDLE_VALUE) {
				return WAIT_FAILED;
			}
			while (state_ == emTSSuspend) {
				Resume();
			}
			return ::WaitForSingleObject(hThread_, ms);
		}

	public:
		unsigned int Run() {
			if (NULL == owner_) {
				return 0;
			}
			bool bRunning = true;
			while (bRunning && InterlockedCompareExchange(&needTerminate_, 1, 1) != 1) {
				bRunning = owner_->WorkThreadProc(this);
			}
			return 0;
		}

	protected:
		static unsigned int __stdcall ThreadProc(void* p) {
			unsigned long ret = 0;
			((Thread*)p)->state_ = emTSRunning;
			ret = ((Thread*)p)->Run();
			((Thread*)p)->Destroy();
			return ret;
		}

	protected:
		ownerT*		owner_;

	protected:
		HANDLE		hThread_;
		long		needTerminate_;
		ThreadState state_;
	};
}

// -------------------------------------------------------------------------
#endif	// __THREAD_H__