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
#include <string.h>
#include <utility>
#include "string.h"
#include "platform.h"
#include "threadmgrbase.h"
#include "log.h"
#include "system.h"

namespace mstd {
    template<typename T>
    class ThreadMgr : public ThreadMgrBase
    {
    private:
	    enum Status {
			kStatusConstruct = 0,
			kStatusInit,
			kStatusInitFailed,
			kStatusRunning,
			kStatusUninit,
			kStatusDestruct
	    };

		struct TimerTrigger {
			TimerTrigger(const uint32_t timer_id = 0, const uint32_t period = 0)
				:timer_id_(timer_id)
				, period_(period)
				, tick_time_(mstd::get_tick_count())
				, is_pending_(false) {
			}

			uint32_t timer_id_;		//定时器Id
			uint64_t period_;		//周期
			uint64_t tick_time_;	//触发时间
			bool     is_pending_;
		};

        struct Command {
            std::function<void()>		func_;                  //cmd仿函数
            const char*		fun_desc_ = nullptr;    //函数描述
            uint32_t        caller_pid_ = 0;        //调用者线程
            uint32_t        start_tick_count_ = 0;
            uint32_t        delay_tick_count_ = 0;
        };
    public:
        ThreadMgr(const char *name):
            ThreadMgrBase(name),
		    thread_id_(0), timer_out_(60 * 1000),
            idle_timeout_(20),
            status_(kStatusConstruct),
            alive_tick_count_(mstd::get_tick_count())
        {
            memset(exec_func_name_, 0x00, sizeof(exec_func_name_));
		    LOGFMTI("ThreadMgr::ThreadMgr() Mgr=%s, this=0x%p", Name().c_str(), this);
        }

	    virtual ~ThreadMgr() {
		    LOGFMTI("ThreadMgr::~ThreadMgr() Mgr=%s, this=0x%p", Name().c_str(), this);
            status_ = kStatusDestruct;
        }

    public:
        bool Init(const uint32_t time_out = 0) {
		    timer_out_ = time_out;
            status_ = kStatusInit;
            thread_ = std::thread(std::bind(&ThreadMgr<T>::threadRoutine, this));
            //获取线程id
            std::stringstream ss;
            ss << thread_.get_id();
            thread_id_ = atoi(ss.str().c_str());

    #ifdef MSTD_WINDOWS
            LOGFMTI("Thread Init Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%I64d.", Name().c_str(), this, thread_id_, alive_tick_count_);
    #else
            LOGFMTI("Thread Init Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%lld.", Name().c_str(), this, thread_id_, alive_tick_count_);
    #endif
            while (status_ == kStatusInit) {
                mstd::sleep(10);
		    }

            if (status_ == kStatusInitFailed) {
			    thread_.join();
			    return false;
            }

		    if (timer_out_){
                RegisterAlive();
            }

            return true;
        }

        /*
        必须在另外的线程空间进行调用，返回后该ThreadMgr的线程也将结束
        */
        void Uninit() {
    #ifdef MSTD_WINDOWS
		    LOGFMTI("Thread Uninit Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%I64d.", Name().c_str(), this, thread_id_, alive_tick_count_);
    #else
            LOGFMTI("Thread Uninit Mgr=%s, this=0x%p, ThreadId=0x%04x. AliveTick=%lld.", Name().c_str(), this, thread_id_, alive_tick_count_);
    #endif
            UnregisterAlive();
			if (kStatusRunning == status_) {
				status_ = kStatusUninit;
				thread_.join();
			}

			do
			{
				std::lock_guard<std::mutex> lock(mutex_);
				cmd_list_.clear();
			} while (0);
			do
			{
				std::lock_guard<std::mutex> lock(mutex_timer_);
				timer_trigger_.clear();
			} while (0);
        }

	    /*
	    注册Timer事件, 派生类会触发OnTimer事件
	    */
	    void RegisterTimer(const uint32_t timer_id, const uint32_t elaspe_ms) {
            TimerTrigger TimTrg(timer_id, elaspe_ms);
		    std::lock_guard<std::mutex> lock(mutex_timer_);
            timer_trigger_[timer_id] = TimTrg;
        }
	    /*
	    取消注册Timer事件
	    */
	    void UnregisterTimer(const uint32_t timer_id) {
		    std::lock_guard<std::mutex> lock(mutex_timer_);
            timer_trigger_.erase(timer_id);
        }

        virtual bool IsAlive(std::string &process_info) {
            uint64_t nTick = mstd::get_tick_count();
            if (nTick - alive_tick_count_ > timer_out_) {
    #ifdef MSTD_WINDOWS
                LOGFMTE("IsVaild Tick=%I64d, Alive=%I64d", nTick, alive_tick_count_);
    #else
                LOGFMTE("IsVaild Tick=%lld, Alive=%lld", nTick, alive_tick_count_);
    #endif
			    process_info = getContext();
                return true;
            }

            return false;
        }

        std::string getContext() {
            std::string context = mstd::format("tid=0x%04x,name=%s,func=%s.", thread_id_, Name().c_str(), exec_func_name_);
            return context;
        }

     protected:
        template<class F, class... Args>
        auto postFunc(F&& f, Args&&... args) -> std::pair<std::future<typename std::invoke_result<F, Args...>::type>, bool> {
            using return_type = typename std::invoke_result<F, Args...>::type;
			auto func = std::make_shared< std::packaged_task<return_type()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            std::future<return_type> res = func->get_future();

            if (status_ != kStatusRunning) {
                return {std::move(res), false};
            }

            std::shared_ptr<Command> cmd = std::make_shared<Command>();
            cmd->func_ = [func]() {
                (*func)();
            };
            cmd->caller_pid_ = mstd::get_current_thread_id();

			// 将任务添加到队列
			if (status_ == kStatusRunning) {
				std::lock_guard<std::mutex> lock(mutex_);
				cmd_list_.push_back(cmd);
                condition_.notify_one();
			}

            return {std::move(res), true };
        }

    private:
	    virtual bool onInit() = 0;
	    virtual void onUninit() = 0;
	    virtual void onIdle() {}
        virtual void onTimer(const uint32_t timer_id) { timer_id; };

    private:
        void threadRoutine(void) {
            try {
                if (this->onInit()) {
				    this->running();
				    this->onUninit();
                } else {
                    status_ = kStatusInitFailed;
                    LOGFMTE("Mgr Init Failed.");
                    return ;
                }
			} catch (std::exception& ex) {
				LOGFMTE("Thread ex=%s", ex.what());
			} catch (...) {
				LOGFMTE("Thread exception");
			}
            return ;
        }

	    void running() {
            status_ = kStatusRunning;
            while (1) {
				if (kStatusRunning != status_)
					break;

				commandLoop();
				onTimerTrigger();
				mstd_strcpy(exec_func_name_, "onIdle");
				onIdle();
				alive_tick_count_ = mstd::get_tick_count();
            }
        }

        void onTimerTrigger() {
		    std::lock_guard<std::mutex> lock(mutex_timer_);
            if (timer_trigger_.empty()) return;

            uint64_t tick_time = mstd::get_tick_count();
            for (auto &[_, trigger] : timer_trigger_) {
                if (tick_time - trigger.tick_time_ > trigger.period_ && !trigger.is_pending_) {
                    trigger.tick_time_ = tick_time;
                    trigger.is_pending_ = true;
                    postFunc(&ThreadMgr<T>::setTimerTrigger, this, trigger.timer_id_);
                }
            }
        }

        bool setTimerTrigger(const uint32_t timer_id_) {
            mstd_sprintf(exec_func_name_, "onTimer tmId=%d", timer_id_);
            onTimer(timer_id_);

			std::lock_guard<std::mutex> lock(mutex_timer_);
			auto it = timer_trigger_.find(timer_id_);
			if (it != timer_trigger_.end()) {
				it->second.is_pending_ = false;
			}
            return true;
        }

	    /*
	    命令循环
	    */
        void commandLoop() {
            bool exec = false;
            do {
				std::unique_lock<std::mutex> lock(mutex_);
                exec = condition_.wait_for(lock, idle_timeout_, [this] {
					       return !cmd_list_.empty(); // 检查条件是否满足
					   });
            } while (0);

            if (!exec) return;

            mutex_.lock();
			auto cmds = std::move(cmd_list_);
            mutex_.unlock();

            for (auto& it : cmds) {
                (it->func_)();
            }
        }

    private:
        uint32_t                                    timer_out_;  //是否注册监控 
	    std::atomic<Status>                         status_;	 //ThreadMgr状态

	    std::thread                                 thread_;       //
	    uint32_t                                    thread_id_;    //线程id

        mutable std::mutex                          mutex_;
        std::list<std::shared_ptr<Command>>         cmd_list_;      //命令队列
	    mutable std::condition_variable             condition_;     //PostCommand调用需要的Cmd事件Handle

        std::chrono::milliseconds                   idle_timeout_;	//每次等待同步事件的超时时长


        uint64_t                                    alive_tick_count_;
	    char                                        exec_func_name_[128];   //当前正在执行的函数

	    mutable std::mutex	                        mutex_timer_;
	    std::map<uint32_t, TimerTrigger>			timer_trigger_;	//以及注册的TimerId的表

	    //disable ctor opreator=
	    ThreadMgr(const ThreadMgr&) = delete;
	    const ThreadMgr& operator=(const ThreadMgr&) = delete;
    };
};


