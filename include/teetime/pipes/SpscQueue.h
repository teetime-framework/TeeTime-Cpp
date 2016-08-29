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

#ifndef TEETIME_CACHELINESIZE
#define TEETIME_CACHELINESIZE 64
#endif

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT

namespace teetime
{
  template<typename T>
  class SpscValueQueue
  {
  public:

    explicit SpscValueQueue(unsigned capacity)
     : m_readIndex(0)
     , m_writeIndex(0)
     , m_array(nullptr)
     , m_capacity(capacity)
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");

      m_array = new Entry[capacity];
    }

    ~SpscValueQueue()
    {
      delete m_array;
    }

    bool write(T&& t)
    {
      auto& entry = m_array[m_writeIndex];

      if (!entry.hasValue.load(std::memory_order_acquire))
      {
        new (&entry.data[0]) T(std::move(t));
        entry.hasValue.store(true, std::memory_order_release);

        if (++m_writeIndex == m_capacity)
        {
          m_writeIndex = 0;
        }
        return true;
      }

      return false;
    }

    bool write(const T& t)
    {
      return write(T(t));
    }

    bool read(T& value)
    {
      auto& entry = m_array[m_readIndex];

      if(entry.hasValue.load(std::memory_order_acquire))
      {
        auto ptr = reinterpret_cast<T*>(&entry.data[0]);
        value = std::move(*ptr);
        ptr->~T();
        entry.hasValue.store(false, std::memory_order_release);

        if(++m_readIndex == m_capacity)
        {
          m_readIndex = 0;
        }  

        return true;
      }

      return false;
    }

    T* frontPtr()
    {
      auto& entry = m_array[m_readIndex];

      if (entry.hasValue.load(std::memory_order_acquire))
      {
        return reinterpret_cast<T*>(&entry.data[0]);
      }

      return nullptr;
    }

    void popFront()
    {
      auto& entry = m_array[m_readIndex];

      assert(entry.hasValue.load(std::memory_order_acquire));

      reinterpret_cast<T*>(&entry.data[0])->~T();
      entry.hasValue.store(false, std::memory_order_release);

      if (++m_readIndex == m_capacity)
      {
        m_readIndex = 0;
      }
    }

    int sizeGuess() const
    {
      int ret = m_writeIndex - m_readIndex;
      if (ret < 0) {
        ret += m_capacity;
      }
      return ret;
    }

  private:
    struct Entry
    {
      Entry()
      {
        hasValue.store(false);
      }

      std::atomic<bool> hasValue;
      char data[sizeof(T)];
    };

    static const size_t CacheLineSize = TEETIME_CACHELINESIZE;

    char padding0[64];
    unsigned m_readIndex;
    char padding1[64 - sizeof(unsigned)];
    unsigned m_writeIndex alignas(CacheLineSize);
    char padding2[64 - sizeof(unsigned)];
    Entry* m_array;
    unsigned m_capacity;
  };


  template<typename T>
  class SpscPointerQueue
  {
  public:

    explicit SpscPointerQueue(unsigned capacity)
      : m_readIndex(0)
      , m_writeIndex(0)
      , m_array(nullptr)
      , m_capacity(capacity)
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");

      m_array = new Entry[capacity];
    }

    ~SpscPointerQueue()
    {
      delete m_array;
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

    static const size_t CacheLineSize = TEETIME_CACHELINESIZE;

    char pagging0[64];
    unsigned m_readIndex;
    char pagging1[64 - sizeof(unsigned)];
    unsigned m_writeIndex;
    char pagging2[64 - sizeof(unsigned)];

    Entry* m_array;
    unsigned m_capacity;
  };

}

TEETIME_WARNING_POP