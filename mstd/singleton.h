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

#ifndef __SINGLETION_H_
#define __SINGLETION_H_

#include <mutex>

namespace mstd
{
    template <typename T>
    class Singleton
    {
    private:
        Singleton();
        ~Singleton();

    public:
        static T& instance();
        static void release();

    private:
        static std::mutex mutex_;
        static T* instance_;
    };

    template <class T>
    std::mutex Singleton<T>::mutex_;

    template <class T>
    T* Singleton<T>::instance_ = NULL;

    template <class T>
    T& Singleton<T>::instance()
    {
        if (instance_ == NULL)
        {
            mutex_.lock();
            if (instance_ == NULL)
                instance_ = new T;
            mutex_.unlock();
        }
        return *instance_;
    }

    template <class T>
    void Singleton<T>::release()
    {
        if (instance_ != NULL)
        {
            mutex_.lock();
            if (instance_ != NULL)
            {
                delete instance_;
                instance_ = NULL;
            }
            mutex_.unlock();
        }
    }

} // namespace mstd

#endif // __SINGLETION_H_
