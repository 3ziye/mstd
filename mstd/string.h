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

/**
* @filename string.h
* @brief 字符串操作
* @author 
* @date 2015年6月26日
* @note
*/

#ifndef __STRING_H_
#define __STRING_H_

#if _MSC_VER >= 1100
#pragma warning(disable: 4996)
#endif

#include <vector>
#include <string>
#include <sstream>
#include <stdarg.h>
#include <stdint.h>
#ifndef MSTD_WINDOWS
#include <fnmatch.h>
#endif


namespace mstd
{
	// 字符串格式化
	std::string format(const char *fmt, ...);
	std::wstring format(const wchar_t *fmt, ...);

	// 格式化日期
	std::string format_datatime(time_t t);

    std::string format_num_thousandth(int64_t num);

	// 字符串分割
	std::vector<std::string> split_string(const std::string &str, const std::string &delim, const bool trim_empty = false);
    std::vector<std::wstring> split_string(const std::wstring &str, const std::wstring &delim, const bool trim_empty = false);
	
	std::string compose_string(const std::vector<std::string> &s, const std::string &delim);
	std::wstring compose_string(const std::vector<std::wstring> &s, const std::wstring &delim);
	// 去除左边空格
    void trim_left(std::string& str);
    void trim_left(std::wstring& str);

	// 去除右边空格
    void trim_right(std::string& str);
    void trim_right(std::wstring& str);

	// 去除前后空格
    void trim(std::string& str);
    void trim(std::wstring& str);

	// 转小写
	std::string to_lower(const std::string& str);
	std::wstring to_lower(const std::wstring& str);

	// 转大写
	std::string to_upper(const std::string& str);
	std::wstring to_upper(const std::wstring& str);

	bool starts_with(const std::string& value, const std::string& start);
	bool ends_with(const std::string& value, const std::string& ending);

	// 字串转HEX
	std::string string_to_hex(const std::string& str);

	// HEX转字符串
	std::string hex_to_string(const std::string& str);

    std::string replace_string(const std::string& source, const std::string& old_value, const std::string& new_value);

    std::wstring replace_string(const std::wstring& source, const std::wstring& old_value, const std::wstring& new_value);

    int string_comparenocase(const std::string &str1, const std::string &str2);
    int string_comparenocase(const std::wstring &str1, const std::wstring &str2);
	bool string_strstrnocase(const std::string &str1, const std::string &str2);

	// 字符串模糊匹配
	int string_fnmatch(const std::string& pattern, const std::string& str);

	// 生成随机字符串
	std::string generator_rand_string(const uint32_t count = 8);

    // 是否utf8字符串
    bool is_utf8(const char* str);

    bool string_compare(const std::string& str1, const std::string& str2, bool caseInsensitive = false);
    bool string_compare(const std::wstring& str1, const std::wstring& str2, bool caseInsensitive = false);
}
#endif	// __STRING_H_