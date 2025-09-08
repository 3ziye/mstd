#pragma once
#include "threadmgr.h"
#include "singleton.h"
#include <set>


class ThreadMonitor : public ThreadMgr<ThreadMonitor>
{
public:
    ThreadMonitor();
    virtual ~ThreadMonitor();

	void RegisteMonitor(ThreadMgrBase* mgr);
    void UnregisteMonitor(ThreadMgrBase* mgr);

    bool IsVaild(std::string &) { return false; }

private:
	virtual bool Init();
	virtual void Uninit();
	virtual void OnIdle();
    virtual void OnTimer(const uint32_t ){};

private:
    uint64_t last_report_tick_;
    uint64_t last_test_tick_;

	mutable std::mutex mutex_;
    std::set<ThreadMgrBase*> set_thread_mgr_;
};

typedef stdxx::Singleton<ThreadMonitor> CSingletonThreadMonitor;
