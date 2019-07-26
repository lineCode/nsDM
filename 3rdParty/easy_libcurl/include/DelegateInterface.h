#pragma once
#include <Windows.h>

class IDelegateInterface
{
public:
	typedef void* DelegateHandle;

public:
	virtual bool start() = 0;
	virtual bool stop(unsigned long timeout) = 0;
	virtual bool post_task(PAPCFUNC, ULONG_PTR data) = 0;
	virtual ~IDelegateInterface() {}
};