

#include "./threadmgrbase.h"
#include "./log.h"
#include "./threadalive.h"

namespace mstd {
    bool ThreadMgrBase::alive_create_ = false;
    uint32_t ThreadMgrBase::mgr_count_ = 0;
    std::recursive_mutex ThreadMgrBase::mutex_;

    ThreadMgrBase::ThreadMgrBase(const char* name)
        :mgr_name_(name) {
        mutex_.lock();
        LOGFMTI("Create Mgr=%s, this=0x%p, MgrCnt=%d.", name, this, mgr_count_ + 1);
        if (!alive_create_) {
            alive_create_ = true;
            SingletonThreadAlive::instance().Create(false);
        }
        ++mgr_count_;
        mutex_.unlock();
    }

    ThreadMgrBase::~ThreadMgrBase() {
        mutex_.lock();
        LOGFMTI("Destroy Mgr=%s, this=0x%p, MgrCnt=%d", mgr_name_.c_str(), this, mgr_count_ - 1);
        if (--mgr_count_ == 1) {
            SingletonThreadAlive::instance().Destroy();
            SingletonThreadAlive::release();
        }
        mutex_.unlock();
    }

    void ThreadMgrBase::RegisterAlive() {
        SingletonThreadAlive::instance().Register(this);
    }

    void ThreadMgrBase::UnregisterAlive() {
        SingletonThreadAlive::instance().Unregister(this);
    }
};