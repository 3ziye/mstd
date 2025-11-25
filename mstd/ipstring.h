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

#ifndef __IP_STRING_H_
#define __IP_STRING_H_

#ifdef MSTD_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif // MSTD_WINDOWS

#include <stddef.h>
#include <cstdio>
#include <algorithm>
#include <string>
#include <stdint.h>

namespace mstd {

std::string ip_to_string(const uint32_t ip);
uint32_t ip_from_string(const std::string &ip);

} // mstd
#endif // #ifndef __IP_STRING_H_
