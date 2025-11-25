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

#pragma once
#include <string>
#include <mutex>

namespace mstd {
    class ThreadMgrBase
    {
    public:
        ThreadMgrBase(const char* name);
        virtual ~ThreadMgrBase();

    public:
        std::string Name() { return mgr_name_; }
        void RegisterAlive();
        void UnregisterAlive();
        virtual bool IsDead(std::string&) { return false; };
    protected:
        std::string mgr_name_;
    private:
        static std::recursive_mutex mutex_;
        static bool alive_create_;
        static uint32_t mgr_count_;
    };

};
