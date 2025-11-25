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

#ifndef __FILE_H_
#define __FILE_H_

#include "platform.h"
#include <string>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(MSTD_WINDOWS)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <utime.h>
#endif

namespace mstd {

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

	bool open(const std::string& path, FileMode mode);
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
#ifdef MSTD_WINDOWS
	HANDLE file_handle_;
#else
	int file_handle_;
#endif
};
}
#endif // __FILE_H_


