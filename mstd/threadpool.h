/*
 * SPDX-FileCopyrightText: Copyright 2025 (C) 3ziye org. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __THREAD_POOL_H_
#define __THREAD_POOL_H_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

namespace mstd {
	class thread_pool {
	public:
		// 构造函数，指定线程数量，默认为硬件支持的并发数
		thread_pool() : stop_(false) {}
		thread_pool(const thread_pool&) = delete;
		thread_pool(thread_pool&&) = default;
		thread_pool& operator=(const thread_pool&) = delete;
		thread_pool& operator=(thread_pool&&) = default;

		// 析构函数，等待所有任务完成
		~thread_pool() {}

		void start(size_t threads = 0) {
			if (threads == 0) {
				threads = std::thread::hardware_concurrency();
			}

			// 创建指定数量的工作线程
			for (size_t i = 0; i < threads; ++i) {
				workers_.emplace_back(
					[this] {
						for (;;) {
							std::function<void()> task;

							// 从任务队列中获取任务
							{
								std::unique_lock<std::mutex> lock(this->mutex_);

								// 等待直到有任务或线程池停止
								this->condition_.wait(lock,
									[this] { return this->stop_ || !this->tasks_.empty(); });

								// 如果线程池已停止且任务队列为空，则退出线程
								if (this->stop_ && this->tasks_.empty()) {
									return;
								}

								// 获取任务
								task = std::move(this->tasks_.front());
								this->tasks_.pop();
							}

							// 执行任务
							task();
						}
					}
				);
			}
		}

		void stop() {
			{
				std::unique_lock<std::mutex> lock(mutex_);
				stop_ = true;
			}

			// 唤醒所有等待的线程
			condition_.notify_all();

			// 等待所有工作线程完成
			for (std::thread& worker : workers_) {
				if (worker.joinable()) {
					worker.join();
				}
			}
		}

		// 提交任务到线程池，支持可变参数
		template<class F, class... Args>
		auto exec(F&& f, Args&&... args)
			-> std::future<typename std::invoke_result<F, Args...>::type> {

			using return_type = typename std::invoke_result<F, Args...>::type;

			// 包装任务为shared_ptr，以便可以被多个线程安全访问
			auto task = std::make_shared< std::packaged_task<return_type()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);

			// 获取与任务关联的future
			std::future<return_type> res = task->get_future();
			{
				std::unique_lock<std::mutex> lock(mutex_);
				// 将任务添加到队列
				if (!stop_)
					tasks_.emplace([task]() { (*task)(); });
			}

			// 唤醒一个等待的线程来执行任务
			condition_.notify_one();
			return res;
		}

	private:
		// 工作线程
		std::vector<std::thread> workers_;
		// 任务队列
		std::queue<std::function<void()>> tasks_;

		// 同步相关
		std::mutex mutex_;
		std::condition_variable condition_;
		bool stop_;
	};
};


#endif

