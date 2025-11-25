#pragma once
#include "threadmgr.h"
#include "singleton.h"
#include <set>

namespace mstd {
    class ThreadAlive : public ThreadMgr<ThreadAlive> {
    public:
		ThreadAlive() : ThreadMgr<ThreadAlive>("ThreadAlive")
			, last_test_tick_(mstd::get_tick_count()) {
		}
        virtual ~ThreadAlive() {}

        void Register(ThreadMgrBase* mgr);
        void Unregister(ThreadMgrBase* mgr);
        
    private:
        virtual bool onInit() { return true; }
        virtual void onUninit() {}
        virtual void onIdle();
        virtual void onTimer(const uint32_t) {};
    private:
        std::mutex mutex_;
        std::set<ThreadMgrBase*> mgrs_;
        uint64_t last_test_tick_;
    };
}
