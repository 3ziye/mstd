
#include "threadalive.h"
#include "log.h"
#include "system.h"

namespace mstd {
    void ThreadAlive::onIdle() {
        uint64_t dwTick = mstd::get_tick_count();
        //10秒检测一次各个注册的ThreadMgr线程运行是否顺畅
        if (dwTick - last_test_tick_ > 10000) {
            std::lock_guard<std::mutex> guard(mutex_);
            for (auto Mgr : mgrs_) {
                std::string info;
                if (Mgr->IsAlive(info)) {
                    LOGFMTE("ThreadAlive Thread TimeOut, info=%s", info.c_str());
                    //exit
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
