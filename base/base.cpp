// base.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "mstd/crc32.h"
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>

namespace fs = std::filesystem;

int main() {
    //crc要调整
    std::string a = "hello world";
    uint32_t crc1 = mstd::crc32::value(a.data(), a.size());
    uint32_t crc2 = mstd::ccbl::crc32_sse((const uint8_t*)a.data(), a.size());

    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件

bool enum_directory(const char* dir, std::vector<std::string>& dirs, std::vector<std::string>& files) {
	try {
		// 检查目录是否存在且是一个有效的目录
		if (!fs::exists(dir) || !fs::is_directory(dir)) {
			return false;
		}

		// 清空输出容器
		dirs.clear();
		files.clear();

		// 遍历目录中的所有条目
		for (const auto& entry : fs::directory_iterator(dir)) {
			// 获取路径的字符串表示
			std::string path_str = entry.path().string();

			if (entry.is_directory()) {
				dirs.push_back(path_str);
			}
			else if (entry.is_regular_file()) {
				files.push_back(path_str);
			}
			// 可以根据需要添加对其他类型文件（如符号链接）的处理
		}

		return true;
	}
	catch (const fs::filesystem_error& e) {
		// 处理文件系统错误（如权限不足）
		return false;
	}
	catch (...) {
		// 处理其他未知错误
		return false;
	}
}

