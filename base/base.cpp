// base.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "mstd/crc32.h"
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include "mstd/threadpool.h"
#include "mstd/system.h"

int task(int id, int sleep_ms) {
	std::cout << "任务 " << id << " 开始执行，线程ID: " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
	std::cout << "任务 " << id << " 执行完成" << std::endl;
	return id * 100;
}

int main() {
    ////crc要调整
    //std::string a = "hello world";
    //uint32_t crc1 = mstd::crc32::value(a.data(), a.size());
    //uint32_t crc2 = mstd::ccbl::crc32_sse((const uint8_t*)a.data(), a.size());

	// 创建线程池，使用默认线程数（硬件并发数）
	mstd::thread_pool pool;
	pool.start(4);

	// 存储任务的future以便获取结果
	std::vector<std::future<int>> results;

	// 提交多个任务到线程池
	for (int i = 0; i < 8; ++i) {
		// 使用run_in_pool提交任务，每个任务休眠时间不同
		results.emplace_back(
			pool.exec(task, i, (i + 1) * 100)
		);
	}

	mstd::sleep(10 * 1000);
	// 等待所有任务完成并输出结果
	std::cout << "\n所有任务结果：" << std::endl;
	for (auto&& result : results) {
		std::cout << result.get() << " ";
	}

	pool.stop();


	std::cout << std::endl;
    std::cout << "Hello World!\n";
}
