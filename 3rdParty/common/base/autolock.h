/* -------------------------------------------------------------------------
//	FileName	£º	autolock.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2015/06/04 ÐÇÆÚËÄ 11:06:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#pragma once

#include "base/non_copyable.h"
#ifndef __BASE_AUTOLOCK_H__
#define __BASE_AUTOLOCK_H__

// -------------------------------------------------------------------------

namespace base {
	template <typename ObjT>
	class AutoLockT: private NonCopyable
	{
	public:
		explicit AutoLockT(ObjT& obj)
			: obj_(obj) {
			obj_.Lock();
		}

		~AutoLockT() {
			obj_.Unlock();
		}

	protected:
		ObjT&	obj_;
	};
}

// -------------------------------------------------------------------------
#endif	// __BASE_AUTOLOCK_H__