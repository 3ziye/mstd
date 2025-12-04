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

#ifndef MSTD_PATH_STRING_H_
#define MSTD_PATH_STRING_H_

#include <string>

namespace mstd
{
    template<typename CharT>
    CharT system_slash();
    template<typename CharT>
    CharT file_suffix();

#ifdef MSTD_WINDOWS
	template<>
	inline char system_slash<char>() { return '\\'; }
	template<>
	inline char file_suffix<char>() { return '.'; }
	template<>
	inline wchar_t system_slash<wchar_t>() { return L'\\'; }
	template<>
	inline wchar_t file_suffix<wchar_t>() { return L'.'; }
#else
	template<>
	inline char system_slash<char>() { return '/'; }
	template<>
	inline char file_suffix<char>() { return '.'; }
	template<>
	inline wchar_t system_slash<wchar_t>() { return L'/'; }
	template<>
	inline wchar_t file_suffix<wchar_t>() { return L'.'; }
#endif

	template<typename CharT>
	CharT reverse_system_slash();

#ifdef MSTD_WINDOWS
	template<>
	inline char reverse_system_slash<char>() { return '/'; }
	template<>
	inline wchar_t reverse_system_slash<wchar_t>() { return L'/'; }
#else
	template<>
	inline char reverse_system_slash<char>() { return '\\'; }
	template<>
	inline wchar_t reverse_system_slash<wchar_t>() { return L'\\'; }
#endif

	template<typename CharT>
	bool is_system_slash(CharT slash)
	{
#ifdef MSTD_WINDOWS
		return
			slash == system_slash<CharT>() ||
			slash == reverse_system_slash<CharT>();
#else
		return slash == system_slash<CharT>();
#endif
	}

	template<typename... Args>
	std::string compose_path(const std::string& base, Args&&... args) {
		std::string path = base;
		auto add_part = [&path](const std::string& part) {
			if (!path.empty() && !is_system_slash<char>(path.back())) {
				path.append(1, system_slash<char>());
			}

			if (!part.empty()) {
				path.append(part);
			}
		};

		(add_part(std::forward<Args>(args)), ...);
		return std::move(path);
	}

	template<typename... Args>
	std::wstring compose_path(const std::wstring& base, Args&&... args) {
		std::wstring path = base;
		auto add_part = [&path](const std::wstring& part) {
			if (!path.empty() && !is_system_slash<wchar_t>(path.back())) {
				path.append(1, system_slash<wchar_t>());
			}

			if (!part.empty()) {
				path.append(part);
			}
		};

		(add_part(std::forward<Args>(args)), ...);
		return std::move(path);
	}


std::string  get_parent_path(const char* path);
std::string  get_parent_path(const std::string& path);
std::wstring get_parent_path(const wchar_t* path);
std::wstring get_parent_path(const std::wstring& path);

std::string  get_last_path(const char* path);
std::string  get_last_path(const std::string& path);
std::wstring get_last_path(const wchar_t* path);
std::wstring get_last_path(const std::wstring& path);

std::string  get_file_suffix(const char* path);
std::string  get_file_suffix(const std::string& path);
std::wstring get_file_suffix(const wchar_t* path);
std::wstring get_file_suffix(const std::wstring& path);

std::string  get_local_sys_path(const char* path);
std::string  get_local_sys_path(const std::string& path);
std::wstring get_local_sys_path(const wchar_t* path);
std::wstring get_local_sys_path(const std::wstring& path);

}  // namespace stdxx

#endif
