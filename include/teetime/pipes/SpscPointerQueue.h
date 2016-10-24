/**
* Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once
#include <atomic>
#include <teetime/platform.h>
#include <cstring>
#include <type_traits>

namespace teetime
{
  template<typename T>
  class SpscPointerQueue
  {
    static_assert(std::is_pointer<T>::value, "T must be a pointer type");

  public:
    explicit SpscPointerQueue(size_t capacity)
      : m_readIndex(0)
      , m_writeIndex(0)
      , m_array(new Entry[capacity])
      , m_capacity(capacity)
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");
    }

    ~SpscPointerQueue()
    {
    }

    bool write(const T& t)
    {
      const auto index = m_writeIndex;
      auto& entry = m_array[index];

      if (!entry.value.load(std::memory_order_acquire))
      {
        entry.value.store(t, std::memory_order_release);

        const auto next = index + 1;
        m_writeIndex = (next == m_capacity) ? 0 : next;

        return true;
      }

      return false;
    }

    bool read(T& value)
    {
      const auto index = m_readIndex;
      auto& entry = m_array[index];

      if (T p = entry.value.load(std::memory_order_acquire))
      {
        value = p;
        entry.value.store(nullptr, std::memory_order_release);

        const auto next = index + 1;
        m_readIndex = (next == m_capacity) ? 0 : next;

        return true;
      }

      return false;
    }

  private:
    struct Entry
    {
      Entry()
      {
        value.store(nullptr);
      }
      std::atomic<T> value;
    };

    char _padding0[platform::CacheLineSize];

    size_t m_readIndex;

    char _padding1[platform::CacheLineSize];

    size_t m_writeIndex;

    char _padding2[platform::CacheLineSize];

    const unique_ptr<Entry[]> m_array;
    const size_t m_capacity;
  };
}
