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
#include "threadmgr.h"
#include "singleton.h"
#include <set>

namespace mstd {
    class ThreadAlive : public ThreadMgr<ThreadAlive> {
    public:
		ThreadAlive() : ThreadMgr<ThreadAlive>("ThreadAlive")
			, last_test_tick_(mstd::get_tick_count()) {
		}
        virtual ~ThreadAlive() {}

        void Register(ThreadMgrBase* mgr);
        void Unregister(ThreadMgrBase* mgr);
        
    private:
        virtual bool onInit() { return true; }
        virtual void onUninit() {}
        virtual void onIdle();
        virtual void onTimer(const uint32_t) {};
    private:
        std::mutex mutex_;
        std::set<ThreadMgrBase*> mgrs_;
        uint64_t last_test_tick_;
    };
}
