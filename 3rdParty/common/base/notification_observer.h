/* -------------------------------------------------------------------------
//	FileName	£º	notification_observer.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2014/11/20 ÐÇÆÚ¶þ 14:38:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#pragma once

#ifndef __BASE_NOTIFICATION_OBSERVER_H__
#define __BASE_NOTIFICATION_OBSERVER_H__
#include "base/non_copyable.h"
// -------------------------------------------------------------------------

namespace base {
	class NotificationObserver: private NonCopyable {
	public:
		virtual void Observe(int type, const void* details, size_t size) = 0;

	protected:
		virtual ~NotificationObserver() {}
	};
}

// -------------------------------------------------------------------------
#endif /* __BASE_NOTIFICATION_OBSERVER_H__ */
