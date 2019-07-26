/* -------------------------------------------------------------------------
//	FileName	��	notification_observer.h
//	Creator		��	yanbozhao
//	CreateTime	��	2014/11/20 ���ڶ� 14:38:11
//	Description	��
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
