
#include "threadalive.h"
#include "log.h"
#include "system.h"

namespace mstd {
    ThreadAlive::ThreadAlive()
        : ThreadMgr<ThreadAlive>("ThreadAlive")
        , last_report_tick_(mstd::get_tick_count())
        , last_test_tick_(mstd::get_tick_count()) {
    }

    ThreadAlive::~ThreadAlive() {
    }

    bool ThreadAlive::onInit() {
        LOGFMTI("ThreadAlive Init");
        return true;
    }

    void ThreadAlive::onUninit() {
        LOGFMTI("ThreadAlive Uninit");
    }

    void ThreadAlive::onIdle() {
        uint64_t dwTick = mstd::get_tick_count();
        if (dwTick - last_report_tick_ > 1000) {
            last_report_tick_ = mstd::get_tick_count();
        }

        //10秒检测一次各个注册的ThreadMgr线程运行是否顺畅
        if (dwTick - last_test_tick_ > 10000) {
            std::unique_lock<std::mutex> guard(mutex_);
            for (auto Mgr : mgrs_) {
                std::string info;
                if (Mgr->IsAlive(info)) {
                    LOGFMTE("ThreadAlive Thread TimeOut, info=%s", info.c_str());
                    mstd::sleep(5000);
                    break;
                }
            }

            last_test_tick_ = mstd::get_tick_count();
        }
    }

    void ThreadAlive::Register(ThreadMgrBase* mgr) {
        mutex_.lock();
        if (this != mgr) {
            mgrs_.insert(mgr);
        }
        mutex_.unlock();
    }

    void ThreadAlive::Unregister(ThreadMgrBase* mgr) {
        mutex_.lock();
        mgrs_.erase(mgr);
        mutex_.unlock();
    }
}
