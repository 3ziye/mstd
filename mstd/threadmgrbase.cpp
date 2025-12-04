

#include "./threadmgr.h"
#include "./log.h"
#include <set>

namespace mstd {
	class ThreadAlive : public ThreadMgr<ThreadAlive> {
	public:
		ThreadAlive() : ThreadMgr<ThreadAlive>("ThreadAlive")
			, last_test_tick_(mstd::get_tick_count()) {
		}
		virtual ~ThreadAlive() {}

		void Register(ThreadMgrBase* mgr) {
			mutex_.lock();
			if (this != mgr) {
				mgrs_.insert(mgr);
			}
			mutex_.unlock();
		}

		void Unregister(ThreadMgrBase* mgr) {
			mutex_.lock();
			mgrs_.erase(mgr);
			mutex_.unlock();
		}

	private:
		virtual bool onInit() { return true; }
		virtual void onUninit() {}
		virtual void onIdle() {
			uint64_t dwTick = mstd::get_tick_count();
			//10秒检测一次各个注册的ThreadMgr线程运行是否顺畅
			if (dwTick - last_test_tick_ > 10000) {
				std::lock_guard<std::mutex> guard(mutex_);
				for (auto Mgr : mgrs_) {
					std::string info;
					if (Mgr->IsDead(info)) {
						LOGFMTE("ThreadAlive Thread TimeOut, info=%s", info.c_str());
						//exit
						break;
					}
				}

				last_test_tick_ = mstd::get_tick_count();
			}
		}
		virtual void onTimer(const uint32_t) {};
	private:
		std::mutex mutex_;
		std::set<ThreadMgrBase*> mgrs_;
		uint64_t last_test_tick_;
	};

    ThreadAlive *thread_alive_;
    bool ThreadMgrBase::alive_create_ = false;
    uint32_t ThreadMgrBase::mgr_count_ = 0;
    std::recursive_mutex ThreadMgrBase::mutex_;

    ThreadMgrBase::ThreadMgrBase(const char* name)
        :mgr_name_(name) {
        mutex_.lock();
        LOGFMTI("Create Mgr=%s, this=0x%p, MgrCnt=%d.", name, this, mgr_count_ + 1);
        if (!alive_create_) {
            alive_create_ = true;
            thread_alive_ = new ThreadAlive();
            thread_alive_->Init();
        }
        ++mgr_count_;
        mutex_.unlock();
    }

    ThreadMgrBase::~ThreadMgrBase() {
        mutex_.lock();
        LOGFMTI("Destroy Mgr=%s, this=0x%p, MgrCnt=%d", mgr_name_.c_str(), this, mgr_count_ - 1);
        if (--mgr_count_ == 1) {
            thread_alive_->Uninit();
            delete thread_alive_;
        }
        mutex_.unlock();
    }

    void ThreadMgrBase::RegisterAlive() {
        thread_alive_->Register(this);
    }

    void ThreadMgrBase::UnregisterAlive() {
        thread_alive_->Unregister(this);
    }
};