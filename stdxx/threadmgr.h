#pragma once
#include <map>
#include <list>
#include <sstream>
#include <future>
#include <thread>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include "threadmgrbase.h"
#include "log.h"
#include "system.h"
#include "stdsafe.h"

#define CreatePostCommand(classname, fun) \
bool PostCommand_##fun()\
{\
    return PostFunc(new ThreadFunc(std::bind(&classname::fun, this)), #fun);\
}

#define CreatePostCommand_Delay(classname, fun) \
bool PostCommand_##fun##_Delay(const uint32_t nDelay, const bool bOnce)\
{\
    return PostFunc(new ThreadFunc(std::bind(&classname::fun, this)), #fun, nDelay, bOnce);\
}

#define CreatePostCommand_N(classname, fun) \
template<typename... Args>\
bool PostCommand_##fun(Args &&... a)\
{\
    return PostFunc(new ThreadFunc(std::bind(&classname::fun, this, std::forward<Args>(a)...)), #fun);\
}

#define CreatePostCommand_N_Delay(classname, fun) \
template<typename... Args>\
bool PostCommand_##fun##_Delay(const uint32_t nDelay, const bool bOnce = false, Args &&... a)\
{\
    return PostFunc(new ThreadFunc(std::bind(&classname::fun, this, std::forward<Args>(a)...)), #fun, nDelay, bOnce);\
}


#define CreateSendCommand(classname, fun) \
    bool SendCommand_##fun()\
{\
    return SendFunc(new ThreadFunc(std::bind(&classname::fun, this)), #fun);\
}

#define CreateSendCommand_N(classname, fun) \
    template<typename... Args>\
    bool SendCommand_##fun(Args &&... a)\
{\
    return SendFunc(new ThreadFunc(std::bind(&classname::fun, this, std::forward<Args>(a)...)), #fun);\
}

struct TimerTrigger
{
    TimerTrigger(const uint32_t timer_id = 0, const uint32_t period = 0)
        :timer_id_(timer_id)
        ,period_(period)
        ,tick_time_(stdxx::get_tick_count())
    {
    }

    uint32_t timer_id_;		//定时器Id
    uint64_t period_;		//周期
    uint64_t tick_time_;	//触发时间
};

template<typename T>
class ThreadMgr : public ThreadMgrBase
{
public:	

    /*
	定义ThreadMgr的状态
	*/
	enum ThreadMgrStatus
	{
		kStatusConstruct = 0,
		kStatusCreating, 
		kStatusInit, 
		kStatusInitFailed, 
		kStatusRunning, 
		kStatusDestroying, 
		kStatusUninit,
		kStatusDestruct
	};

    typedef std::function<bool(T&)> ThreadFunc;

    struct CommandArgs
    {
        enum CmdOptions
        {
            SendCommand = 1,
            PostCommand = 2,
        };

        ThreadFunc*		thread_func_ = nullptr;         //cmd仿函数
        const char*		fun_desc_ = nullptr;			//函数描述
        uint32_t        own_thread_id_ = 0;				//调用者线程
        uint32_t        cmd_option_ = PostCommand;
        uint32_t        start_tick_count_ = 0;
        uint32_t        delay_tick_count_ = 0;

        bool HasOptions(uint32_t option) const { return ((cmd_option_ & option) != 0); } 
    };

    ThreadMgr(const char *name):
        ThreadMgrBase(name),
		m_thread_id_(0), timer_out_(60 * 1000),
        idle_timeout_(20), high_water_mark_(512),
        thread_mgr_status_(kStatusConstruct),
        alive_tick_count_(stdxx::get_tick_count())
    {
        memset(exec_func_name_, 0x00, sizeof(exec_func_name_));
        memset(exec_func_detail_, 0x00, sizeof(exec_func_detail_));

		LOGFMTI("ThreadMgr::ThreadMgr() Mgr=%s, this=0x%p", GetMgrName().c_str(), this);
    }

	virtual ~ThreadMgr()
    {
		LOGFMTI("ThreadMgr::~ThreadMgr() Mgr=%s, this=0x%p", GetMgrName().c_str(), this);
        thread_mgr_status_ = kStatusDestruct;
    }

public:
    bool Create(const uint32_t time_out = 60 *1000)
    {
		timer_out_ = time_out;
        thread_mgr_status_ = kStatusCreating;
        m_thread_ = std::thread(std::bind(&ThreadMgr<T>::ThreadMgrRoutine, this));

        //获取线程id
        std::stringstream ss;
        std::thread::id id = m_thread_.get_id();
        ss << id;
        m_thread_id_ = atoi(ss.str().c_str());

#ifdef WIN32
        LOGFMTI("Thread Create Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%I64d.", GetMgrName().c_str(), this, m_thread_id_, alive_tick_count_);
#else
        LOGFMTI("Thread Create Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%lld.", GetMgrName().c_str(), this, m_thread_id_, alive_tick_count_);
#endif
        while (thread_mgr_status_ == kStatusCreating || thread_mgr_status_ == kStatusInit)
		{
			stdxx::sleep(10);
		}

        if (thread_mgr_status_ == kStatusInitFailed)
        {
			m_thread_.join();
			return false;
        }

		if (timer_out_)
		{
            RegisteMonitor();
        }

        return true;
    }

    /*
    必须在另外的线程空间进行调用，返回后该ThreadMgr的线程也将结束
    */
    void Destroy()
    {
		LOGFMTI("Thread Destroy Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%I64d.", GetMgrName().c_str(), this, m_thread_id_, alive_tick_count_);

        UnregisteMonitor();

        do 
        {
			std::unique_lock<std::mutex> lock(mutex_destroy_);
            if (thread_mgr_status_ == kStatusRunning)
            {
                thread_mgr_status_ = kStatusDestroying;
				m_thread_.join();
            }

        } while(0);

        ClearAllCmd();
        FreeAllTimer();
    }

    ThreadMgrStatus Status() const { return thread_mgr_status_; }
    void Status(ThreadMgrStatus s) { thread_mgr_status_ = s;  }

	/*
	注册Timer事件, 派生类会触发OnTimer事件
	*/
	void RegisteTimer(const uint32_t timer_id, const uint32_t elaspe_ms)
    {
        TimerTrigger TimTrg(timer_id, elaspe_ms);
		std::unique_lock<std::mutex> lock(mutex_timer_);
        timer_trigger_[timer_id] = TimTrg;
    }
	
	/*
	取消注册Timer事件
	*/
	void UnregisteTimer(const uint32_t timer_id)
    {
		std::unique_lock<std::mutex> lock(mutex_timer_);
        timer_trigger_.erase(timer_id);
    }

    virtual bool IsVaild(std::string &process_info)
    {
        uint64_t nTick = stdxx::get_tick_count();
        if (nTick - alive_tick_count_ > timer_out_)
        {
#ifdef WIN32
            LOGFMTE("IsVaild Tick=%I64d, Alive=%I64d", nTick, alive_tick_count_);
#else
            LOGFMTE("IsVaild Tick=%lld, Alive=%lld", nTick, alive_tick_count_);
#endif
			process_info = GetCurrentProcessInfo();
            return true;
        }

        return false;
    }

    virtual void KeepVaild() {
        alive_tick_count_ = stdxx::get_tick_count();
    }

    void SetFuncDetail(const char *func_detail)
    {
		strncpy(exec_func_detail_, func_detail, sizeof(exec_func_detail_));
    }

    char *GetCurrentProcessInfo()
    {
        static char info[256] = {0};
		safe_sprintf(info, sizeof(info), "ThreadId=0x%04x,Name=%s,Func=%s,Detail=%s.", m_thread_id_, GetMgrName().c_str(), exec_func_name_, exec_func_detail_);
        return info;
    }

    bool PostCommand_Lambda(const std::function<void()> &func, const char *fun_name = "Lambda",  const uint32_t delay = 0) {
        return PostFunc(new ThreadFunc(std::bind(&ThreadMgr<T>::ProcessLambda, this, func)), fun_name, delay);
    }

 protected:
	/*
	提供一个同步运行命令的方法，对该Mgr提交一个命令
	命令的执行在该Mgr的线程空间中运行，并且触发ProcessCommand接口
	调用者函数会一直等待该Cmd在ProcessCommand执行完毕后才返回
	*/
	bool SendFunc(ThreadFunc *func, const char *desc)
    {
        if (thread_mgr_status_ != kStatusRunning && thread_mgr_status_ != kStatusInit)
        {
            delete func;
            return false;
        }

        bool result = false;

        CommandArgs cmd;
        cmd.thread_func_ = func;
        cmd.fun_desc_ = desc;
        cmd.own_thread_id_ = stdxx::get_current_thread_id();
        cmd.cmd_option_ = CommandArgs::SendCommand;

        if (cmd.own_thread_id_ == m_thread_id_)
        {
            T *pThis = (T*)this;
            uint64_t nBeginTime = stdxx::get_tick_count();
            result = (*cmd.thread_func_)(*pThis);
            delete cmd.thread_func_;
            uint64_t nSpendTime = stdxx::get_tick_count() - nBeginTime;
            if (nSpendTime > 500)
            {
                LOGFMTW("SendFunc ThreadMgr[%s] Process Func[%s] TimeOut Spend %d ms",
                    GetMgrName().c_str(), desc, nSpendTime);
            }
        }
        else
        {
            std::unique_lock <std::mutex> lock(mutex_sync_);
            send_result_ = false;
            mutex_async_.lock();
            cmd_list_.push_front(cmd);
            async_condition_.notify_one();
			mutex_async_.unlock();

            if (kStatusDestroying > thread_mgr_status_)
            {
				//线程未销毁则循环等待任务的完成
				std::chrono::milliseconds wait_milliseconds(-1);
                sync_condition_.wait(lock);

                result = send_result_;
            }
            else
            {
                return false;
            }
        }

        return result;
    }

	/*
	提供一个异步运行命令的方法，对该Mgr提交一个命令
	命令的执行在该Mgr的线程空间中运行，并且触发ProcessCommand接口
	调用者函数会马上返回
	*/
    bool PostFunc(ThreadFunc *func, const char *desc, const uint32_t delay_ms = 0, const bool once = false) //nDelayMinSec延迟执行的毫秒数
    {
        if (thread_mgr_status_ != kStatusRunning && thread_mgr_status_ != kStatusInit)
        {
            delete func;
            return false;
        }

        CommandArgs cmd;
        cmd.thread_func_ = func;
        cmd.fun_desc_ = desc;
        cmd.own_thread_id_ = stdxx::get_current_thread_id();
        cmd.cmd_option_ = CommandArgs::PostCommand;
        if (delay_ms)
        {
            cmd.start_tick_count_ = (uint32_t)stdxx::get_tick_count();
            cmd.delay_tick_count_ = delay_ms;   //延迟一定时间再执行
        }
        
		mutex_async_.lock();
        if (delay_ms && once)
        {
            //对于只执行一次的函数看任务队列里有没有函数，有的话直接返回了
            for (auto &it : cmd_list_)
            {
                if (0 == strcmp(cmd.fun_desc_, it.fun_desc_))
                {
                    mutex_async_.unlock();
                    return false;
                }
            }
        }

        cmd_list_.push_back(cmd);
        if ((uint32_t)cmd_list_.size() > high_water_mark_)
        {
            LOGFMTW("Mgr[%04x %s] PostCommand MoreCmdCount=%d,Cmd[%s],ExecCmd[%s].",
                m_thread_id_, GetMgrName().c_str(), (uint32_t)cmd_list_.size(), desc, exec_func_name_);
        }

		mutex_async_.unlock();
        async_condition_.notify_one();
        return true;
    }

    void SetIdleTimeout(const std::chrono::milliseconds time_ms)
    {
        idle_timeout_ = time_ms;
    }

private:
    /*
	纯虚接口，在Create调用的时候会运行，运行空间在该Mgr的线程空间中运行
	派生类必须实现该接口
	*/
	virtual bool Init() = 0;

	/*
	纯虚接口，在Destroy调用的时候会运行，运行空间在该Mgr的线程空间中运行
	派生类必须实现该接口
	*/
	virtual void Uninit() = 0;

	/*
	纯虚接口，在Mgr创建成功后，当命令队列为空的时候会定时触发该接口
	*/
	virtual void OnIdle(){}

	/*
	虚接口，在运行RegisteTimer会触发OnTimer函数
	用到RegisteTimer的派生类应该实现该接口
	*/
    virtual void OnTimer(const uint32_t timer_id) { timer_id; };

private:
    void ThreadMgrRoutine(void)
    {
        uint32_t init_step = 0;
        try
        {
            if (this->Init())
            {
                init_step++;
				this->Status(ThreadMgr::kStatusInit);
                init_step++;
				this->Running();
                init_step++;
				this->Uninit();
                init_step++;
				this->Status(ThreadMgr::kStatusUninit);
                init_step++;
				this->UnregisteMonitor();
                init_step++;
            }
            else
            {
				this->UnregisteMonitor();
				this->Status(ThreadMgr::kStatusInitFailed);

                LOGFMTE("Mgr Init Failed.");
                return ;
            }
        }
        catch (...)
        {
            LOGFMTE("Mgr Expection");
        }

        return ;
    }

	void Running()
    {
        thread_mgr_status_ = kStatusRunning;
        while (1)
        {
            try
            {
                OnTimerTrigger();

                if (!CommandLoop())
                    break;

				safe_strcpy(exec_func_name_, "OnIdle", sizeof(exec_func_name_));
                OnIdle();
                alive_tick_count_ = stdxx::get_tick_count();
            }
            catch (std::exception &ex) {
                LOGFMTE("Thread in Running ex=%s", ex.what());
            }
            catch (...)
            {
                LOGFMTE("Thread exception");
            }
        }
    }

    void OnTimerTrigger()
    {
		std::unique_lock<std::mutex> lock(mutex_timer_);
        if (timer_trigger_.empty()) return;

        uint64_t tick_time = stdxx::get_tick_count();
        for (auto &Trigger : timer_trigger_)
        {
            if (tick_time - Trigger.second.tick_time_ > Trigger.second.period_)
            {
                PostFunc(new ThreadFunc(std::bind(&ThreadMgr<T>::SetTimerTrigger, this, Trigger.second.timer_id_)), "TimerTrigger");
                Trigger.second.tick_time_ = tick_time;
            }
        }
    }

    bool SetTimerTrigger(const uint32_t timer_id_)
    {
		safe_sprintf(exec_func_name_, sizeof(exec_func_name_),  "OnTimer TimerId=%d", timer_id_);
        OnTimer(timer_id_);
        return true;
    }

	/*
	命令循环
	*/
    bool CommandLoop()
    {
        if (kStatusDestroying == thread_mgr_status_)
        {
            return false;
        }

        bool is_empty = false;

        do 
        {
            std::unique_lock<std::mutex> lock(mutex_async_);
            async_condition_.wait_for(lock, idle_timeout_);
            is_empty = cmd_list_.empty();
        } while (0);


        T *pThis = (T*)this;

        std::vector<CommandArgs> delay_cmds_;
        while (!is_empty)
        {
			mutex_async_.lock();
            uint64_t begin_time = stdxx::get_tick_count();
            CommandArgs cmd = *cmd_list_.begin();
            cmd_list_.pop_front();
            if (cmd.delay_tick_count_)
            {
                if (begin_time - cmd.start_tick_count_ < cmd.delay_tick_count_)
                {
                    //需要延迟执行的时间还没到
                    delay_cmds_.push_back(cmd);
                    is_empty = cmd_list_.empty();
                    mutex_async_.unlock();
                    continue;
                }
            }

            is_empty = cmd_list_.empty();
			mutex_async_.unlock();

			safe_strcpy(exec_func_name_, cmd.fun_desc_, sizeof(exec_func_name_));
            exec_func_detail_[0] = '\0';
            
            bool result = (*cmd.thread_func_)(*pThis);
            uint64_t spend_time = stdxx::get_tick_count() - begin_time;
            if (spend_time > 500)
            {
                LOGFMTW("ThreadMgr[%s] Process Func[%s] TimeOut, Spend %d ms,OwnThreadId=0x%04x.",
                    GetMgrName().c_str(), exec_func_name_, (uint32_t)spend_time, cmd.own_thread_id_);
            }

            if (cmd.HasOptions(CommandArgs::SendCommand))
            {
                send_result_ = result;
                sync_condition_.notify_one();
            }

            delete cmd.thread_func_;

            alive_tick_count_ = stdxx::get_tick_count();
        }

        if (!delay_cmds_.empty())
        {
            //没到延迟执行的命令压回队列
            std::unique_lock<std::mutex> lock(mutex_async_);
            for (auto &cmd : delay_cmds_)
            {
                cmd_list_.push_back(cmd);
            }
        }

        return true;
    }

	void FreeAllTimer()
    {
		std::unique_lock<std::mutex> lock(mutex_timer_);
        timer_trigger_.clear();
    }

    void ClearAllCmd()
    {
		std::unique_lock<std::mutex> lock(mutex_async_);
        for (auto &cmd : cmd_list_)
        {
            delete cmd.thread_func_;
        }
        
        cmd_list_.clear();
    }

    bool ProcessLambda(const std::function<void()> &func) {
        func();
        return true;
    }

private:
    uint32_t                                    timer_out_;  //是否注册监控 
	std::atomic<ThreadMgrStatus>                thread_mgr_status_;			//ThreadMgr状态

	std::thread                                 m_thread_;       //
	uint32_t                                    m_thread_id_;     //线程id

    mutable std::mutex                          mutex_sync_;
	mutable std::condition_variable             sync_condition_;//SendCommand调用需要的同步事件Handle
    bool    send_result_;										//SendCommand执行的返回值

    mutable std::mutex                          mutex_async_;
    std::list<CommandArgs>                      cmd_list_;          //命令队列
	mutable std::condition_variable             async_condition_; //PostCommand调用需要的Cmd事件Handle

    std::chrono::milliseconds                   idle_timeout_;	//每次等待同步事件的超时时长


    uint64_t                                    alive_tick_count_;
	char                                        exec_func_name_[128];   //当前正在执行的函数
    char                                        exec_func_detail_[256];

	std::mutex                                  mutex_destroy_; 
    uint32_t                                    high_water_mark_;   //命令高水位

	mutable std::mutex	                        mutex_timer_;
	std::map<uint32_t, TimerTrigger>			timer_trigger_;	//以及注册的TimerId的表

	//disable ctor opreator=
	ThreadMgr(const ThreadMgr&) = delete;
	const ThreadMgr& operator=(const ThreadMgr&) = delete;
};

namespace stdxx {
    template<typename Task>
    void execute_simple_task(const Task &task) {
        //std::async(std::launch::deferred, task);
        std::thread t = std::thread(std::bind(task));
        t.detach();
    }

};


