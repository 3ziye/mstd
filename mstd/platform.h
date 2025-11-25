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

 #ifndef __PLATFORM_H_
#define __PLATFORM_H_
#include <stdio.h>
#include <stdint.h>
//MSTD_WINDOWS MSTD_LINUX MSTD_UNIX MSTD_APPLE
namespace mstd {
	template <typename T = void>
	struct is_x64 {
		static constexpr bool value = sizeof(T*) == 8;
	};
	
	bool is_little_endian();
	bool is_big_endian();

	uint16_t htons(uint16_t host_value);
	uint32_t htonl(uint32_t host_value);
	uint64_t htonll(uint64_t host_value);
	uint16_t ntohs(uint16_t net_value);
	uint32_t ntohl(uint32_t net_value);
	uint64_t ntohll(uint64_t net_value);

	template<typename T>
	constexpr T hton(T value) {
		//static_assert(std::is_integral_v<T>, "Only integral types are supported");
		if constexpr (sizeof(T) == 1) {
			return value; // 单字节不需要转换
		} else if constexpr (sizeof(T) == 2) {
			return htons(static_cast<uint16_t>(value));
		} else if constexpr (sizeof(T) == 4) {
			return htonl(static_cast<uint32_t>(value));
		} else if constexpr (sizeof(T) == 8) {
			return htonll(static_cast<uint64_t>(value));
		}

		static_assert(sizeof(T) <= 8, "Unsupported type size");
		return value;
	}

	template<typename T>
	constexpr T ntoh(T value) {
		return hton(value);
	}
};

#ifdef MSTD_WINDOWS
#define mstd_sprintf(buffer, fmt, ...) sprintf_s(buffer, sizeof(buffer), fmt, __VA_ARGS__)
#define mstd_strcpy(buffer, src) strcpy_s(buffer, sizeof(buffer), src)
#else
#define mstd_sprintf(buffer, fmt, ...) snprintf(buffer, sizeof(buffer), fmt, __VA_ARGS__)
#define mstd_strcpy(buffer, src) strncpy(buffer, src, sizeof(buffer))
#define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#endif // __PLATFORM_DEFINE_H_
