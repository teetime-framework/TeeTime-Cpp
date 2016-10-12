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

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT

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
      , m_capacity(static_cast<unsigned>(capacity))
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");
    }

    ~SpscPointerQueue()
    {
    }

    bool write(const T& t)
    {
      auto& entry = m_array[m_writeIndex];

      if (!entry.value.load(std::memory_order_acquire))
      {
        entry.value.store(t, std::memory_order_release);

        if (++m_writeIndex == m_capacity)
        {
          m_writeIndex = 0;
        }
        return true;
      }

      return false;
    }

    bool read(T& value)
    {
      auto& entry = m_array[m_readIndex];

      if (T p = entry.value.load(std::memory_order_acquire))
      {
        value = p;
        entry.value.store(nullptr, std::memory_order_release);

        if (++m_readIndex == m_capacity)
        {
          m_readIndex = 0;
        }

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

    unsigned m_readIndex;

    char _padding1[platform::CacheLineSize];

    unsigned m_writeIndex;

    char _padding2[platform::CacheLineSize];

    unique_ptr<Entry[]> m_array;
    unsigned m_capacity;
  };


  namespace v2
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
        , m_capacity(static_cast<unsigned>(capacity))
        , m_mask(static_cast<unsigned>(capacity) - 1)
      {
        assert(m_capacity >= 2 && "queue capacity must be at least 2");
      }

      ~SpscPointerQueue()
      {
      }

      bool write(const T& t)
      {
        unsigned index = m_writeIndex;
        auto& entry = m_array[index];

        if (!entry.value.load(std::memory_order_acquire))
        {
          entry.value.store(t, std::memory_order_release);

          ++index;
          if (index == m_capacity)
          {
            m_writeIndex = 0;
          }
          else
          {
            m_writeIndex = index;
          }
          return true;
        }

        return false;
      }

      bool read(T& value)
      {
        unsigned index = m_readIndex;
        auto& entry = m_array[index];

        if (T p = entry.value.load(std::memory_order_acquire))
        {
          value = p;
          entry.value.store(nullptr, std::memory_order_release);

          ++index;
          if (index == m_capacity)
          {
            m_readIndex = 0;
          }
          else
          {
            m_readIndex = index;
          }

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

      unsigned m_readIndex;

      char _padding1[platform::CacheLineSize];

      unsigned m_writeIndex;

      char _padding2[platform::CacheLineSize];

      const unique_ptr<Entry[]> m_array;
      const unsigned m_capacity;
      const unsigned m_mask;
    };
  }

  namespace v3
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
        , m_capacity(static_cast<unsigned>(capacity))
        , m_mask(static_cast<unsigned>(capacity)-1)
      {
        assert(m_capacity >= 2 && "queue capacity must be at least 2");
      }

      ~SpscPointerQueue()
      {
      }

      bool write(const T& t)
      {
        const unsigned index = m_writeIndex;
        auto& entry = m_array[index];

        if (!entry.value.load(std::memory_order_acquire))
        {
          entry.value.store(t, std::memory_order_release);
          m_writeIndex = (index + 1) & m_mask;
          return true;
        }

        return false;
      }

      bool read(T& value)
      {
        const unsigned index = m_readIndex;
        auto& entry = m_array[index];

        if (T p = entry.value.load(std::memory_order_acquire))
        {
          value = p;
          entry.value.store(nullptr, std::memory_order_release);
          m_readIndex = (index + 1) & m_mask;
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

      unsigned m_readIndex;

      char _padding1[platform::CacheLineSize];

      unsigned m_writeIndex;

      char _padding2[platform::CacheLineSize];

      const unique_ptr<Entry[]> m_array;
      const unsigned m_capacity;
      const unsigned m_mask;
    };
  }


  namespace v4
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
        , m_capacity(static_cast<size_t>(capacity))
        , m_mask(static_cast<size_t>(capacity) - 1)
      {
        assert(m_capacity >= 2 && "queue capacity must be at least 2");
      }

      ~SpscPointerQueue()
      {
      }

      bool write(const T& t)
      {
        const size_t index = m_writeIndex;
        auto& entry = m_array[index];

        if (!entry.value.load(std::memory_order_acquire))
        {
          entry.value.store(t, std::memory_order_release);
          m_writeIndex = (index + 1) & m_mask;
          return true;
        }

        return false;
      }

      bool read(T& value)
      {
        const size_t index = m_readIndex;
        auto& entry = m_array[index];

        if (T p = entry.value.load(std::memory_order_acquire))
        {
          value = p;
          entry.value.store(nullptr, std::memory_order_release);
          m_readIndex = (index + 1) & m_mask;
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
      const size_t m_mask;
    };
  }
}

TEETIME_WARNING_POP