
#pragma once

#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>

struct disk_info
{
	std::string disk_name;
	uint64_t disk_size;
	uint64_t disk_used;
	uint64_t disk_avail;
	std::string disk_capacity;
	std::string disk_mounted;
};

namespace mstd
{
// 从操作系统启动所经过（elapsed）的毫秒数
uint64_t get_tick_count();
// 从操作系统启动所经过（elapsed）的微秒数
int64_t get_tick_count_ms();
// 返回cpu核心数
uint32_t get_cpu_cores();
//获取当前进程id
uint32_t get_current_process_id();
//获取当前线程id
uint32_t get_current_thread_id();
//暂停线程单位：毫秒
void sleep(const uint32_t milliseconds);
//结束进程
bool kill_processes_by_name(const char* process_name);
//运行系统命令
bool system_cmd(const std::string &cmd);
//运行系统命令返回执行结果
std::string shell_cmd(const std::string &cmd);

//获取进程全路径
bool get_process_path(std::string& filename);
bool get_process_path(std::wstring& filename);

template<typename Tchar>
std::basic_string<Tchar> get_process_path() {
    std::basic_string<Tchar> str;
	get_process_path(str);
    return str;
}


//获取进程文件夹路径
bool get_program_dir(std::string& path);
bool get_program_dir(std::wstring& path);

template<typename Tchar>
std::basic_string<Tchar> get_program_dir()
{
    std::basic_string<Tchar> str;
	get_program_dir(str);
    return str;
}

std::string get_last_error();

// 获取Linux/Unix磁盘信息
int get_disk_info(std::vector<disk_info> &vec_disk_info);

} 


