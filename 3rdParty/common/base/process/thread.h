/* -------------------------------------------------------------------------
//	FileName	£º	thread.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015/05/06 ÐÇÆÚÈý 15:42:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#ifndef __BASE_THREAD_H__
#define __BASE_THREAD_H__

#include <windows.h>
#include <process.h>
// -------------------------------------------------------------------------

namespace base {
	class Thread
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
			, state_(emTSNormal) {
		}

		virtual ~Thread() {
			Destroy();
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
			DWORD ret = SuspendThread(hThread_);
			if (ret != -1) {
				state_ = emTSSuspend;
			}
			return ret != -1;
		}

		bool Resume() {
			if (hThread_ == INVALID_HANDLE_VALUE) {
				return false;
			}
			DWORD ret = ResumeThread(hThread_);
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
		DWORD WaitForEnd(unsigned long ms = INFINITE) {
			if (hThread_ == INVALID_HANDLE_VALUE) {
				return WAIT_FAILED;
			}
			while (state_ == emTSSuspend) {
				Resume();
			}
			return ::WaitForSingleObject(hThread_, ms);
		}

	public:
		virtual UINT Run() = 0;

	protected:
		static UINT __stdcall ThreadProc(void* p) {
			UINT ret = 0;
			((Thread*)p)->state_ = emTSRunning;
			ret = ((Thread*)p)->Run();
			((Thread*)p)->Destroy();
			return ret;
		}

	protected:
		Thread(const Thread& rhs);
		Thread& operator=(const Thread& rhs);

	protected:
		HANDLE		hThread_;
		LONG		needTerminate_;
		ThreadState state_;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_THREAD_H__