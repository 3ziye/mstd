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
        virtual bool OnInit();
        virtual void OnUninit();
        virtual void OnIdle();
        virtual void OnTimer(const uint32_t) {};

    private:
        std::mutex mutex_;
        std::set<ThreadMgrBase*> mgrs_;
        uint64_t last_report_tick_;
        uint64_t last_test_tick_;
    };

    typedef mstd::Singleton<ThreadAlive> SingletonThreadAlive;
}
