
#include "threadmonitor.h"
#include "log.h"
#include "system.h"
#include <Windows.h>
ThreadMonitor::ThreadMonitor()
    : ThreadMgr<ThreadMonitor>("ThreadMonitor")
    ,last_report_tick_(stdxx::get_tick_count())
    ,last_test_tick_(stdxx::get_tick_count())
{
}

ThreadMonitor::~ThreadMonitor()
{
}

bool ThreadMonitor::Init()
{
    return true;
}

void ThreadMonitor::Uninit()
{
    LOGFMTI("TestAliveMgr Uninit");
}

void ThreadMonitor::OnIdle() {
	uint64_t dwTick = stdxx::get_tick_count();
    if (dwTick - last_report_tick_ > 1000) {
		last_report_tick_ = stdxx::get_tick_count();
	}

    //10秒检测一次各个注册的ThreadMgr线程运行是否顺畅
    if (dwTick - last_test_tick_ > 10000) {
        std::string strInfo;
        std::unique_lock<std::mutex> guard(mutex_);
        for (auto Mgr : set_thread_mgr_) {
            if (Mgr->IsVaild(strInfo)) {
                LOGFMTE("ThreadMonitor Thread TimeOut, Info=%s", strInfo.c_str());
                stdxx::sleep(5000);
                ::TerminateProcess(GetCurrentProcess(), -1);
                break;
            }
        }

        last_test_tick_ = stdxx::get_tick_count();
    }
}


void ThreadMonitor::RegisteMonitor(ThreadMgrBase* mgr)
{
	mutex_.lock();
	if (this != mgr)
	{
        set_thread_mgr_.insert(mgr);
	}
	mutex_.unlock();
}

void ThreadMonitor::UnregisteMonitor(ThreadMgrBase* mgr)
{
    mutex_.lock();
	set_thread_mgr_.erase(mgr);
    mutex_.unlock();
}
