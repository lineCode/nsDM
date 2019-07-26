/* -------------------------------------------------------------------------
//	FileName	:	criticalsection.h
//	Creator		:	yanbozhao
//	CreateTime	:	2015/05/06 ÐÇÆÚÈý 15:42:11
//	Description	:
//
// -----------------------------------------------------------------------*/
#pragma once

#ifndef __BASE_CRITICALSECTION_H__
#define __BASE_CRITICALSECTION_H__

#include <windows.h>
#include "base/non_copyable.h"
// -------------------------------------------------------------------------

namespace base {
	class CriticalSection: private NonCopyable
	{
	public:
		CriticalSection() {
			::InitializeCriticalSectionAndSpinCount(&cs_, 5000);
		}

		~CriticalSection() {
			::DeleteCriticalSection(&cs_);
		}

	public:
		void CriticalSection::Lock() {
			::EnterCriticalSection(&cs_);
		}

		void CriticalSection::Unlock() {
			::LeaveCriticalSection(&cs_);
		}

	protected:
		CRITICAL_SECTION cs_;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_CRITICALSECTION_H__