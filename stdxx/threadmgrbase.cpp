

#include "./threadmgrbase.h"
#include "./log.h"
#include "./threadmonitor.h"

bool ThreadMgrBase::self_creating_ = false;
unsigned int ThreadMgrBase::self_count_ = 0;
std::recursive_mutex ThreadMgrBase::mutex_;

ThreadMgrBase::ThreadMgrBase(const char *name)
    :mgr_name_(name)
{
	mutex_.lock();
    LOGFMTI("Create Mgr=%s, this=0x%p, MgrCnt=%d.", name, this, self_count_ + 1);

    if (!self_creating_)
    {
        self_creating_ = true;
        CSingletonThreadMonitor::instance().Create(false);
    }

    ++self_count_;
	mutex_.unlock();
}

ThreadMgrBase::~ThreadMgrBase()
{
    //must after destroy fucntion has called
    mutex_.lock();
    LOGFMTI("Destroy Mgr=%s, this=0x%p, MgrCnt=%d", mgr_name_.c_str(), this, self_count_ - 1);
    if (--self_count_ == 1)
    {
        CSingletonThreadMonitor::instance().Destroy();
        CSingletonThreadMonitor::release();
    }
    mutex_.unlock();
}

void ThreadMgrBase::RegisteMonitor()
{
    KeepVaild();
    CSingletonThreadMonitor::instance().RegisteMonitor(this);
}

void ThreadMgrBase::UnregisteMonitor()
{
    CSingletonThreadMonitor::instance().UnregisteMonitor(this);
}
