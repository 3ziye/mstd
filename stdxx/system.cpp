
#include "system.h"
#include <chrono>
#include <thread>
#include <sstream>
#include "string_convert.h"
#include "path_string.h"
#ifdef WIN32
#include <Windows.h>
#include <TlHelp32.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#if defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <sys/user.h>
#endif

#if defined(__APPLE__)
#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>
#endif


namespace stdxx
{
uint64_t get_tick_count()
{
#ifdef WIN32
    return GetTickCount();
#else
    using namespace std::chrono;
    steady_clock::duration elapsed = steady_clock::now().time_since_epoch();
    return duration_cast<milliseconds>(elapsed).count();
#endif
}

int64_t get_tick_count_ms()
{
// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
#define EPOCHFILETIME   (11644473600000000ULL)
#ifdef WIN32
 	FILETIME ft;
 	LARGE_INTEGER li;
 	int64_t mseconds = 0;
    //GetSystemTimePreciseAsFileTime(&ft);
    GetSystemTimeAsFileTime(&ft);
 	li.LowPart = ft.dwLowDateTime;
 	li.HighPart = ft.dwHighDateTime;
 	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
 	mseconds = (li.QuadPart - EPOCHFILETIME) / 10;
 	return mseconds;
#else
	timeval tv;
	gettimeofday(&tv, 0);
	return (int64_t)tv.tv_sec * 1000000 + (int64_t)tv.tv_usec;
#endif // WIN32
	return 0;
}

uint32_t get_cpu_cores()
{
    return std::thread::hardware_concurrency();
}

//获取当前进程id
uint32_t get_current_process_id() {
#ifdef WIN32
    return GetCurrentProcessId();
#elif defined(linux) || defined(__linux) || defined(__linux__)
    return getpid();
#else

#endif
}

uint32_t get_current_thread_id()
{
#ifdef WIN32
    return GetCurrentThreadId();

#elif defined(linux) || defined(__linux) || defined(__linux__)
	return pthread_self();
#else
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	std::string stid = oss.str();
	unsigned long long tid = std::stoull(stid);
	return tid;
#endif
}

//
void sleep(const uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

bool kill_processes_by_name(const char* process_name) 
{
#ifdef WIN32
	HANDLE hSnapshot = INVALID_HANDLE_VALUE;
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hSnapshot)
	{
		return false;
	}

	PROCESSENTRY32W pe = { sizeof(PROCESSENTRY32) };

	bool bFind = false;
	bool bProcess = Process32FirstW(hSnapshot, &pe) ? true : false;
	while (bProcess)
	{
		if (0 == wcsicmp(pe.szExeFile, stdxx::to_wstring(process_name).c_str()))
		{
			HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
			if (hProcess)
			{
				::TerminateProcess(hProcess, 0);
			}
			CloseHandle(hProcess);
			bFind = true;
			break;
		}

		bProcess = Process32NextW(hSnapshot, &pe) ? true : false;
	}
	::CloseHandle(hSnapshot);

	return bFind;
#else
	bool result = true;

	DIR *dir = NULL;
	struct dirent *next;
	int BUFFER_SIZE = 256;

	// open /proc
	dir = opendir("/proc");
	if (!dir) {
		return -1;
	}

	while ((next = readdir(dir)) != NULL) {
		FILE *fp_status = NULL;
		char filename[BUFFER_SIZE];
		char buffer[BUFFER_SIZE];
		char name[BUFFER_SIZE];

		/* Must skip ".." since that is outside /proc */
		if (strcmp(next->d_name, "..") == 0)
			continue;

		sprintf(filename, "/proc/%s/status", next->d_name);
		if (!(fp_status = fopen(filename, "r"))) {
			continue;
		}
		if (fgets(buffer, BUFFER_SIZE - 1, fp_status) == NULL) {
			fclose(fp_status);
			continue;
		}
		fclose(fp_status);

		/* Buffer should contain a string like "Name:   binary_name" */
		sscanf(buffer, "%*s %s", name);

		if ((strstr(name, process_name)) != NULL) {
			// Trying to kill
			int signal = kill(strtol(next->d_name, NULL, 0), SIGKILL);
			if (signal != 0) {
				fprintf(stderr, "Unable to kill process %s (%s)\n", name, next->d_name);
				result = false;
			}
		}
	}
	closedir(dir);
	return result;
#endif
}

bool system_cmd(const std::string &cmd)
{
	return (system(cmd.c_str()) == 0);
}

std::string shell_cmd(const std::string &cmd)
{
#define BUFFER_SIZE 1024
	char buffer[BUFFER_SIZE];
	std::string ret;

#if defined(_WIN32)
	FILE* stream = _popen(cmd.c_str(), "r");
#else 
	FILE* stream = popen(cmd.c_str(), "r");
#endif

	if (stream != NULL) {
		while (fgets(buffer, BUFFER_SIZE, stream))
			ret += buffer;
#if defined(_WIN32)
		_pclose(stream);
#else
		pclose(stream);
#endif
	}

	return ret;
}

bool get_process_filename(std::string& filename)
{
	char the_filename[260] = { 0 };
#ifdef WIN32
    DWORD length = ::GetModuleFileNameA(nullptr, the_filename, MAX_PATH);
#elif defined(linux) || defined(__linux) || defined(__linux__)
	std::size_t length = readlink("/proc/self/exe", the_filename, sizeof(the_filename) - 1);
#elif defined(__FreeBSD__)
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	size_t length = sizeof(the_filename);
	sysctl(mib, 4, the_filename, &length, NULL, 0);
#elif defined(__APPLE__)
	size_t length = 0;
	_NSGetExecutablePath(nullptr, &length);
	if (length <= 1) {
		return false;
	}

	std::string executable_path(length - 1, std::string::value_type());
	int rv = _NSGetExecutablePath(&executable_path[0], &length);
	if (rv != 0) {
		return false;
	}
	filename = executable_path;
#endif
    if (length > 0 && length < 260)
    {
        filename = the_filename;
        return true;
    }

    return false;
}

bool get_process_filename(std::wstring& filename)
{
	const int kMaxPath = 1024;  // unicode support 32767
	wchar_t the_filename[kMaxPath];
#ifdef WIN32
    DWORD length = ::GetModuleFileNameW(nullptr, the_filename, kMaxPath);
#elif defined(linux) || defined(__linux) || defined(__linux__)
	char buffer[kMaxPath];
	std::size_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	std::wstring wstr = stdxx::to_wstring(buffer);
	wstr.copy(the_filename, wstr.length() > sizeof(the_filename) - 1 ? sizeof(the_filename) - 1 : wstr.length(), 0);
#elif defined(__FreeBSD__)
	char buffer[kMaxPath];
	int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };
	size_t length = sizeof(buffer);
	sysctl(mib, 4, buffer, &length, NULL, 0);
	std::wstring wstr = stdxx::to_wstring(buffer);
	wstr.copy(the_filename, wstr.length() > sizeof(the_filename) - 1 ? sizeof(the_filename) - 1 : wstr.length(), 0);
#elif defined(__APPLE__)
	size_t length = 0;
	_NSGetExecutablePath(nullptr, &length);
	if (length <= 1) {
		return false;
}

	std::string executable_path(length - 1, std::string::value_type());
	int rv = _NSGetExecutablePath(&executable_path[0], &length);
	if (rv != 0) {
		return false;
	}
	std::wstring wstr = stdxx::to_wstring(executable_path);
	wstr.copy(the_filename, wstr.length() > sizeof(the_filename) - 1 ? sizeof(the_filename) - 1 : wstr.length(), 0);
#endif
    if (length > 0 && length < kMaxPath)
    {
        filename = the_filename;
        return true;
    }

    return false;
}


bool get_program_path(std::string& path) {
    std::string filename;
    if (get_process_filename(filename)) {
        std::string::size_type pos = filename.rfind(stdxx::system_slash<char>());
        if (pos != std::string::npos) {
            path = filename.substr(0, pos);
            return true;
        }
    }
    return false;
}

bool get_program_path(std::wstring& path) {
    std::wstring filename;
    if (get_process_filename(filename)) {
        std::wstring::size_type pos = filename.rfind(stdxx::system_slash<wchar_t>());
        if (pos != std::wstring::npos) {
            path = filename.substr(0, pos);
            return true;
        }
    }
    return false;
}

std::string get_last_error() {
#ifdef WIN32
    return strerror(GetLastError());
#else
    return strerror(errno);
#endif
}

int get_disk_info(std::vector<disk_info>& vec_disk_info)
{
#ifndef WIN32
	std::string disk_info_str = shell_cmd("/bin/df");
	std::istringstream input(disk_info_str);
	for (std::string line; std::getline(input, line); ) {
		if (strncmp(line.c_str(), "/dev", 4) == 0) {
			disk_info info;
			info.disk_name.resize(64);
			info.disk_capacity.resize(16);
			info.disk_mounted.resize(32);
			sscanf(line.c_str(), "%s %llu %llu %llu %s %s", &info.disk_name[0], &info.disk_size, &info.disk_used, &info.disk_avail, &info.disk_capacity[0], &info.disk_mounted[0]);
			vec_disk_info.push_back(info);
		}
	}
#endif
	return vec_disk_info.size();
}


}  
