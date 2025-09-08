
#include "threadpool.h"
#include "platform_define.h"
#include <time.h>
#include <deque>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include "system.h"


#if defined(STDXX_WINDOWS)
	#include <windows.h>
	#include <process.h>
#elif defined(STDXX_LINUX) || defined(STDXX_MACOS)
	#include <pthread.h>
#endif

namespace stdxx
{
namespace threads
{
	//class pool
		//struct task
			pool::task::task(type k) : kind(k){}
			pool::task::~task(){}
		//end struct task

		//struct task_signal
		struct pool::task_signal
			: task
		{
			task_signal()
				: task(task::signal)
			{}

			void run()
			{}
		};//end struct task_signal

		class pool::impl
		{
			enum class state{init, idle, run, finished};

			struct pool_thread_obj
			{
#if defined(STDXX_WINDOWS)
				typedef HANDLE thread_t;
#elif defined(STDXX_POSIX)
				typedef pthread_t thread_t;
#endif
				impl * pool_ptr;
				task * task_ptr;
				thread_t	handle;
				std::atomic<state>	thr_state;
				time_t	timestamp;
#if defined(STDXX_POSIX)
				std::mutex wait_mutex;
				std::condition_variable wait_cond;
				std::atomic<bool> suspended;
#endif
			};
		public:
			impl(std::size_t thr_number)
			{
				if(0 == thr_number) thr_number = 4;

				for(; thr_number; --thr_number)
				{
					pool_thread_obj * pto = new pool_thread_obj;
					pto->pool_ptr = this;
					pto->thr_state = state::init;
					pto->task_ptr = nullptr;
#if defined(STDXX_WINDOWS)
					pto->handle = (HANDLE)::_beginthreadex(0, 0, reinterpret_cast<unsigned(__stdcall*)(void*)>(&impl::thread_starter), pto, 0, 0);
#elif defined(STDXX_POSIX)
					pto->suspended = false;
					::pthread_create(&(pto->handle), 0, reinterpret_cast<void*(*)(void*)>(&impl::thread_starter), pto);
#endif
					container_.threads.emplace_back(pto);
				}
			}

			~impl()
			{
				runflag_ = false;

				while(true)
				{
					bool all_finished = true;
					{
						for(auto thr: container_.threads)
						{
							if(state::finished != thr->thr_state)
							{
								all_finished = false;
								break;
							}
						}
					}

					if(all_finished)
						break;

					while(true)
					{
						auto idle_thr = get_thread_idle();
						if(idle_thr)
							thread_resume(idle_thr);
						else
							break;
					}
					stdxx::sleep(100);
				}

				std::vector<pool_thread_obj*> dup(std::move(container_.threads));

				for(auto thr: dup)
				{
#if defined(STDXX_WINDOWS)
					::WaitForSingleObject(thr->handle, INFINITE);
					::CloseHandle(thr->handle);
#elif defined(STDXX_POSIX)
					::pthread_join(thr->handle, 0);
					::pthread_detach(thr->handle);
#endif
				}

				std::lock_guard<decltype(mutex_)> lock(mutex_);
				for(auto task_ptr : container_.tasks)
				{
					delete task_ptr;
				}
			}

			void push(task * taskptr)
			{
				if(false == runflag_)
				{
					delete taskptr;
					throw std::runtime_error("Nana.Pool: Do not accept task now");
				}

				pool_thread_obj * pto = get_thread_idle();

				if(pto)
				{
					pto->task_ptr = taskptr;
					thread_resume(pto);
				}
				else
				{
					std::lock_guard<decltype(mutex_)> lock(mutex_);
					container_.tasks.emplace_back(taskptr);
				}
			}

			void wait_for_signal()
			{
				std::unique_lock<std::mutex> lock(signal_.mutex);
				signal_.cond.wait(lock);
			}

			void wait_for_finished()
			{
				while(true)
				{
					{
						std::lock_guard<decltype(mutex_)> lock(mutex_);
						if(container_.tasks.empty())
						{
							bool finished = true;
							for(auto thr : container_.threads)
							{
								if(state::run == thr->thr_state)
								{
									finished = false;
									break;
								}
							}
							if(finished)
								return;
						}
					}
					stdxx::sleep(100);
				}
			}
		private:
			pool_thread_obj* get_thread_idle()
			{
				for(auto thr : container_.threads)
					if(state::idle == thr->thr_state)
					{
						std::lock_guard<decltype(mutex_)> lock(mutex_);
						if(state::idle == thr->thr_state)
						{
							thr->thr_state = state::run;
							return thr;
						}
					}
				return nullptr;
			}

			void thread_suspend(pool_thread_obj* pto)
			{
				pto->thr_state = state::idle;
#if defined(STDXX_WINDOWS)
				::SuspendThread(pto->handle);
#elif defined(STDXX_POSIX)
				std::unique_lock<std::mutex> lock(pto->wait_mutex);
				pto->suspended = true;
				pto->wait_cond.wait(lock);
				pto->suspended = false;
#endif
			}

			void thread_resume(pool_thread_obj* pto)
			{
#if defined(STDXX_WINDOWS)
				while(true)
				{
					DWORD n = ::ResumeThread(pto->handle);
					if(n == 1 || n == static_cast<DWORD>(-1))
						break;
				}
#elif defined(STDXX_POSIX)
				while(false == pto->suspended)
					;
				std::unique_lock<std::mutex> lock(pto->wait_mutex);
				pto->wait_cond.notify_one();
#endif
			}

			bool thread_read(pool_thread_obj* pto)
			{
				pto->task_ptr = nullptr;
				if(runflag_)
				{
					std::lock_guard<decltype(mutex_)> lock(mutex_);
					if(container_.tasks.size())
					{
						pto->task_ptr = container_.tasks.front();
						container_.tasks.pop_front();
					}
				}
				else
					return false;

				if(nullptr == pto->task_ptr)
				{
					//The task queue is empty, so that
					//suspend and wait for a task.
					thread_suspend(pto);
				}

				return (nullptr != pto->task_ptr);
			}

			void thread_runner(pool_thread_obj* pto)
			{
				while(thread_read(pto))
				{
					pto->timestamp = time(nullptr);
					switch(pto->task_ptr->kind)
					{
					case task::general:
						try
						{
							pto->task_ptr->run();
						}catch(...){}
						break;
					case task::signal:
						while(true)
						{
							bool finished = true;
							{
								std::lock_guard<decltype(mutex_)> lock(mutex_);
								for(auto thr : container_.threads)
								{
									if((thr != pto) && (state::run == thr->thr_state) && (thr->timestamp <= pto->timestamp))
									{
										finished = false;
										break;
									}
								}
							}

							if(finished)
								break;
							stdxx::sleep(100);
						}

						//wait till the cond is waiting.
						signal_.cond.notify_one();
						break;
					}
					delete pto->task_ptr;
					pto->task_ptr = nullptr;
				}

				pto->thr_state = state::finished;
			}

			//Here defines the a function used for creating a thread.
			//This is platform-specified.
#if defined(STDXX_WINDOWS)
			static unsigned __stdcall thread_starter(pool_thread_obj * pto)
			{
				pto->pool_ptr->thread_runner(pto);
				::_endthreadex(0);
				return 0;
			}
#elif defined(STDXX_POSIX)
			static void * thread_starter(pool_thread_obj * pto)
			{
				pto->pool_ptr->thread_runner(pto);
				return nullptr;
			}
#endif
		private:
			std::atomic<bool> runflag_{ true };
			std::recursive_mutex mutex_;

			struct signal
			{
				std::mutex mutex;
				std::condition_variable cond;
			}signal_;

			struct container
			{
				std::deque<task*> tasks;
				std::vector<pool_thread_obj*> threads;
			}container_;
		};//end class impl

		pool::pool()
			: impl_(new impl(4))
		{
		}

		pool::pool(pool&& other)
			: pool()
		{
			std::swap(impl_, other.impl_);
		}

		pool::pool(std::size_t thread_number)
			: impl_(new impl(thread_number))
		{
		}

		pool& pool::operator=(pool&& other)
		{
			if(this != &other)
			{
				delete impl_;
				impl_ = other.impl_;
				other.impl_ = new impl(4);
			}
			return *this;
		}

		pool::~pool()
		{
			delete impl_;
		}

		void pool::signal()
		{
			task * task_ptr = nullptr;
			try
			{
				task_ptr = new task_signal;
				push_task(task_ptr);
			}
			catch(std::bad_alloc&)
			{
				delete task_ptr;
			}
		}

		void pool::wait_for_signal()
		{
			impl_->wait_for_signal();
		}

		void pool::wait_for_finished()
		{
			impl_->wait_for_finished();
		}

		void pool::push_task(task* task_ptr)
		{
			impl_->push(task_ptr);
		}
	//end class pool

}//end namespace threads
}//end namespace stdxx
