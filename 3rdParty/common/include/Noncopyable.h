/* -------------------------------------------------------------------------
//	FileName	£º	non_copyable.h
//	Creator		£º	luoning@yy.com
//	CreateTime	£º	2016/04/01 ÐÇÆÚÎå 16:19:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#pragma once

namespace base
{

class NonCopyable
{
protected:
	NonCopyable(){}
	~NonCopyable(){}

private:
	NonCopyable(const NonCopyable&);
	
	NonCopyable& operator=(const NonCopyable&);

#if __cplusplus >= 201103L
	NonCopyable(NonCopyable&&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;
#endif
	
};
}