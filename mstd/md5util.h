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

#ifndef __MD5UTIL_H_
#define __MD5UTIL_H_

#include <string>

namespace mstd
{
	/**
	 * @brief md5_string
	 * @note 获取字符串md5
	 * @param str
	 * @param md5_hash
	 * @return
	 */
	bool md5_string(const std::string& str, std::string& md5);

	/**
	 * @brief md5_string
	 * @note 获取字符串md5
	 * @param str
	 * @return
	 */
	std::string md5_string(const std::string& str);

	/**
	 * @brief md5_file
	 * @note 获取文件md5
	 * @param filename
	 * @param md5_hash
	 * @return
	 */
	bool md5_file(const char* filename, std::string& md5);

	/**
	 * @brief md5_file
	 * @note 获取文件md5
	 * @param filename
	 * @return
	 */
	std::string md5_file(const char* filename);
};

#endif //__MD5UTIL_H_
