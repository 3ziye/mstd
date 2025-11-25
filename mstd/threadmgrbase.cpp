

#include "./threadmgrbase.h"
#include "./log.h"
#include "./threadalive.h"

namespace mstd {
    ThreadAlive thread_alive_;
    bool ThreadMgrBase::alive_create_ = false;
    uint32_t ThreadMgrBase::mgr_count_ = 0;
    std::recursive_mutex ThreadMgrBase::mutex_;

    ThreadMgrBase::ThreadMgrBase(const char* name)
        :mgr_name_(name) {
        mutex_.lock();
        LOGFMTI("Create Mgr=%s, this=0x%p, MgrCnt=%d.", name, this, mgr_count_ + 1);
        if (!alive_create_) {
            alive_create_ = true;
            thread_alive_.Init();
        }
        ++mgr_count_;
        mutex_.unlock();
    }

    ThreadMgrBase::~ThreadMgrBase() {
        mutex_.lock();
        LOGFMTI("Destroy Mgr=%s, this=0x%p, MgrCnt=%d", mgr_name_.c_str(), this, mgr_count_ - 1);
        if (--mgr_count_ == 1) {
            thread_alive_.Uninit();
        }
        mutex_.unlock();
    }

    void ThreadMgrBase::RegisterAlive() {
        thread_alive_.Register(this);
    }

    void ThreadMgrBase::UnregisterAlive() {
        thread_alive_.Unregister(this);
    }
};