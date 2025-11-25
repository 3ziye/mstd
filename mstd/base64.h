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

#ifndef __BASE64_H_
#define __BASE64_H_

#include <cstring>
#include <string>

namespace mstd {

  std::string base64_encode(const char* s, std::size_t n);
  std::string base64_decode(const char* s, std::size_t n);

  inline std::string base64_encode(const char* s) {
    return base64_encode(s, std::strlen(s));
  }

  inline std::string base64_decode(const char* s) {
    return base64_decode(s, std::strlen(s));
  }

  inline std::string base64_encode(const std::string& s) {
    return base64_encode(s.data(), s.size());
  }

  inline std::string base64_decode(const std::string& s) {
    return base64_decode(s.data(), s.size());
  }
}

#endif // __BASE64_H_
