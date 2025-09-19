// base.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "mstd/log.h"
#include "mstd/system.h"
#include "mstd/path_string.h"
#include "mstd/fileutils.h"
#include "mstd/threadpool.h"
#include "mstd/system.h"
#include "mstd/ipstring.h"
#include "mstd/crc32.h"
#include "mstd/serialize.h"
#include "ProcessMgr.h"

void initLog() {
	std::string dir = mstd::get_program_dir<char>();
	std::string log_dir = mstd::compose_path(dir, "log");
	mstd::verify_directory(log_dir);
	mstd::log::set_directory(log_dir.c_str());
	mstd::log::set_level(mstd::log::Info);
	mstd::log::set_reserve_days(5);
}

void test_thread_pool();

struct Body {
	uint32_t id;
	std::string Name;
	std::vector<std::string> Friends;
	mypack_serialize(id, Name, Friends);
};

int main() {
	Body b;
	b.Name = "dc";
	b.id = 1;
	b.Friends.push_back("yc");
	b.Friends.push_back("wq");

	ByteBuffer buff;
	b.serialize(buff);

	Body c;
	c.deserialize(buff);

    ////crc要调整
    std::string a = "hello world";
    uint32_t crc1 = mstd::crc32::value(a.data(), a.size());
    uint32_t crc2 = mstd::ccbl::crc32_sse((const uint8_t*)a.data(), a.size());
	initLog();

	uint32_t ip = mstd::ip_from_string("192.168.31.2");
	std::string ips = mstd::ip_to_string(ip);

	std::function<void()> f = std::bind(&test_thread_pool);
	std::string name = f.target_type().name();

	ProcessMgr mgr;
	mgr.Init();
	mgr.RegisterTimer(1, 200);

	int resutl = mgr.Process1(1, 2);
	LOGFMTI("result1=%d", resutl);
	mgr.Process2(2);

	mstd::sleep(10 * 1000);
	mgr.Uninit();

	std::cout << std::endl;
    std::cout << "Hello World!\n";
}



int task(int id, int sleep_ms) {
	std::cout << "任务 " << id << " 开始执行，线程ID: " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
	std::cout << "任务 " << id << " 执行完成" << std::endl;
	return id * 100;
}

void task2() {

}

void test_thread_pool() {
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

	pool.exec(task2);
	//mstd::sleep(10 * 1000);
	// 等待所有任务完成并输出结果
	std::cout << "\n所有任务结果：" << std::endl;
	for (auto&& result : results) {
		std::cout << result.get() << " ";
	}

	pool.stop();
}

template <typename Ret, typename... Args>
class function_ex {
private:
	std::function<Ret(Args...)> func;  // 被包装的函数
	std::string name;                  // 函数名称（手动指定）

public:
	// 构造函数：接收函数和名称
	function_ex(std::function<Ret(Args...)> f, std::string n)
		: func(std::move(f)), name(std::move(n)) {}

	// 重载调用运算符：打印信息并调用原函数
	Ret operator()(Args... args) const {
		// 打印函数名和参数
		std::cout << "调用函数: " << name << "(";
		print_args(args...);  // 递归打印参数
		std::cout << ")" << std::endl;

		// 调用原函数并返回结果
		Ret result = func(args...);
		std::cout << "返回值: " << result << std::endl;  // 打印返回值
		return result;
	}

private:
	// 递归终止函数（无参数时）
	static void print_args() {}

	// 递归打印参数（展开参数包）
	template <typename First, typename... Rest>
	static void print_args(First first, Rest... rest) {
		std::cout << first;
		if constexpr (sizeof...(rest) > 0) {
			std::cout << ", ";
			print_args(rest...);  // 递归处理剩余参数
		}
	}
};