/* -------------------------------------------------------------------------
//	FileName	£º	notification_service.h
//	Creator		£º	yanbozhao
//	CreateTime	£º	2014/11/20 ÐÇÆÚ¶þ 14:38:11
//	Description	£º
//
// -----------------------------------------------------------------------*/
#pragma once

#ifndef __BASE_NOTIFICATION_SERVICE_H__
#define __BASE_NOTIFICATION_SERVICE_H__

#include "notification_observer.h"
#include <vector>
#include "base/non_copyable.h"
#include "base/autolock.h"
#include "base/synchronization/criticalsection.h"
// -------------------------------------------------------------------------

namespace base {
	class NotificationService: private NonCopyable {
	public:
		static NotificationService* current() {
			static NotificationService notify_service;
			return &notify_service;
		}

		NotificationService() {}
		~NotificationService() {}

		virtual void Notify(int type, const void* details, size_t size) {
			//base::AutoLockT<base::CriticalSection> lock(observerSection_);
			std::vector<NotificationObserver*>::iterator it = notify_observer_list_.begin();
			for (it; it != notify_observer_list_.end(); ++it) {
				if (*it) {
					(*it)->Observe(type, details, size);
				}
			}
		}

		void AddObserver(NotificationObserver* observer) {
			//base::AutoLockT<base::CriticalSection> lock(observerSection_);
			if (observer) {
				notify_observer_list_.push_back(observer);
			}
		}
		void RemoveObserver(NotificationObserver* observer) {
			//base::AutoLockT<base::CriticalSection> lock(observerSection_);
			std::vector<NotificationObserver*>::iterator it = notify_observer_list_.begin();
			while (it != notify_observer_list_.end()) {
				if ((*it) == observer) {
					it = notify_observer_list_.erase(it);
		}
				else {
					++it;
		}
			}
		}

	private:
		std::vector<NotificationObserver*>	notify_observer_list_;
		base::CriticalSection observerSection_;
	};
}
// -------------------------------------------------------------------------

#endif /* __BASE_NOTIFICATION_SERVICE_H__ */

