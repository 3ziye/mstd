#pragma once
#include "threadmgr.h"
#include "singleton.h"
#include <set>

namespace mstd {
    class ThreadAlive : public ThreadMgr<ThreadAlive>
    {
    public:
        ThreadAlive();
        virtual ~ThreadAlive();

        void Register(ThreadMgrBase* mgr);
        void Unregister(ThreadMgrBase* mgr);
        bool IsAlive(std::string&) { return false; }
    private:
        virtual bool onInit();
        virtual void onUninit();
        virtual void onIdle();
        virtual void onTimer(const uint32_t) {};

    private:
        std::mutex mutex_;
        std::set<ThreadMgrBase*> mgrs_;
        uint64_t last_report_tick_;
        uint64_t last_test_tick_;
    };

    typedef mstd::Singleton<ThreadAlive> SingletonThreadAlive;
}
