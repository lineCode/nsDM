/* -------------------------------------------------------------------------
//	FileName	��	non_copyable.h
//	Creator		��	luoning@yy.com
//	CreateTime	��	2016/04/01 ������ 16:19:11
//	Description	��
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