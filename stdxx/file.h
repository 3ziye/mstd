#ifndef __FILE_H_
#define __FILE_H_

#include "platform_define.h"
#include <string>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(STDXX_WINDOWS)
#include <windows.h>
#elif defined(STDXX_POSIX)
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#endif

namespace stdxx {

class file
{
public:
	enum FileMode
	{
		ReadMode = 0,
		WriteMode,
		ReadWrite,
	};

	enum FileSeek
	{
		FileBegin = 0,
		FileCurrent,
		FileEnd,
	};

	file();
	virtual ~file();
	//utf-8 path
    std::string path();
	//Linux/Unix utf-8
	bool open(const std::string& path, FileMode mode);
	//Windows UNICODE
	bool open(const std::wstring& path, FileMode mode);
	std::size_t read(char* buffer, std::size_t size);
	std::size_t write(const char* buffer, std::size_t size);
	uint64_t seek(uint64_t size, FileSeek seek);
	bool length(uint64_t size);
    uint64_t length();
    bool last_modify_time(time_t tm);
    time_t   last_modify_time();
	void close();

private:
	std::string file_path_;
#if defined(STDXX_WINDOWS)
	HANDLE file_handle_;
#elif defined(STDXX_POSIX)
	int file_handle_;
#endif
};
}
#endif // __FILE_H_


