#pragma once
#include <string>
#include <mutex>

class ThreadMgrBase
{
public:
    ThreadMgrBase(const char *name);
    virtual ~ThreadMgrBase();

public:
	void RegisteMonitor();
	void UnregisteMonitor();

    virtual bool IsVaild(std::string &strInfo) = 0;
    virtual void KeepVaild() = 0;
    std::string GetMgrName(){ return mgr_name_; }
protected:
    std::string mgr_name_;
private:
    static std::recursive_mutex mutex_;
    static bool self_creating_;
	static uint32_t self_count_;
};

