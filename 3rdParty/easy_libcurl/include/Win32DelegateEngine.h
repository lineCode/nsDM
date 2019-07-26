#pragma once
#include "DelegateInterface.h"
#include <Windows.h>

class Win32DelegateEngine :
	public IDelegateInterface
{
public:
	Win32DelegateEngine(void);
	virtual ~Win32DelegateEngine(void);

	virtual bool start();

	virtual bool stop(unsigned long timeout);

	virtual bool post_task(PAPCFUNC apc_fun, ULONG_PTR data);

private:
	static unsigned int __stdcall delegate_engine( void * );
	static VOID CALLBACK wakeup_apc(_In_ ULONG_PTR dwParam);

private:
	DelegateHandle _handle;
	LONG _stop_flag;
};

