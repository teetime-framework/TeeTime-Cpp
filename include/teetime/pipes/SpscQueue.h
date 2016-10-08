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
  class SpscValueQueue
  {
  public:

    explicit SpscValueQueue(size_t capacity)
     : m_readIndex(0)
     , m_writeIndex(0)
     , m_array(new Entry[capacity])
     , m_capacity(static_cast<unsigned>(capacity))
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");
    }

    ~SpscValueQueue()
    {
    }

    bool write(T&& t)
    {
      auto& entry = m_array[m_writeIndex];

      if (!entry.hasValue.load(std::memory_order_acquire))
      {
        new (entry.ptr()) T(std::move(t));
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
        auto ptr = entry.ptr();
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
        return entry.ptr();
      }

      return nullptr;
    }

    void popFront()
    {
      auto& entry = m_array[m_readIndex];

      assert(entry.hasValue.load(std::memory_order_acquire));

      entry.ptr()->~T();
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

      T* ptr()
      {
        return reinterpret_cast<T*>(&data[0]);
      }

      const T* ptr() const
      {
        return reinterpret_cast<const T*>(&data[0]);
      }

      std::atomic<bool> hasValue;

    private:      
      char data[sizeof(T)] alignas(T);
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
#if defined(__GNUC__) && __GNUC__ < 5
    template<typename T>
    struct trivial_copy
    {
      static const bool value = __has_trivial_copy(T);
    };
#else
    template<typename T>
    using trivial_copy = std::is_trivially_copyable<T>;
#endif
    template<bool trivial>
    struct helper;

    template<>
    struct helper<false>
    {
      template<typename T>
      static void constructAt(void* p, T&& t)
      {
        new (p) T(std::move(t));
      }
    };

    template<>
    struct helper<true>
    {
      template<typename T>
      static void constructAt(void* p, T&& t)
      {
        memcpy(p, &t, sizeof(T));
      }
    };

    template<typename T>
    void constructAt(T&& t, void* p)
    {
      helper<trivial_copy<T>::value>::constructAt(p, std::move(t));
    }

    template<typename T>
    class SpscValueQueue
    {
    public:

      explicit SpscValueQueue(size_t capacity)
        : m_readIndex(0)
        , m_writeIndex(0)
        , m_array(new Entry[capacity])
        , m_capacity(static_cast<unsigned>(capacity))
      {
        assert(m_capacity >= 2 && "queue capacity must be at least 2");
      }

      ~SpscValueQueue()
      {
      }

      bool write(T&& t)
      {
        const unsigned int index = m_writeIndex;
        auto& entry = m_array[index];

        if (!entry.hasValue.load(std::memory_order_acquire))
        {
          constructAt<T>(std::move(t), entry.ptr());
          entry.hasValue.store(true, std::memory_order_release);
          const auto next = index + 1;
          m_writeIndex = (next != m_capacity) ? next : 0;
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
        const unsigned int index = m_readIndex;
        auto& entry = m_array[index];

        if (entry.hasValue.load(std::memory_order_acquire))
        {
          auto ptr = entry.ptr();
          value = std::move(*ptr);
          ptr->~T();
          entry.hasValue.store(false, std::memory_order_release);
          const auto next = index + 1;
          m_readIndex = (next != m_capacity) ? next : 0;
          return true;
        }

        return false;
      }

      T* frontPtr()
      {
        auto& entry = m_array[m_readIndex];

        if (entry.hasValue.load(std::memory_order_acquire))
        {
          return entry.ptr();
        }

        return nullptr;
      }

      void popFront()
      {
        auto& entry = m_array[m_readIndex];

        assert(entry.hasValue.load(std::memory_order_acquire));

        entry.ptr()->~T();
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

        T* ptr()
        {
          return reinterpret_cast<T*>(&data[0]);
        }

        const T* ptr() const
        {
          return reinterpret_cast<const T*>(&data[0]);
        }
      
        alignas(T) char data[sizeof(T)];
        std::atomic<bool> hasValue;
      };

      char _padding0[platform::CacheLineSize];

      unsigned m_readIndex;

      char _padding1[platform::CacheLineSize];

      unsigned m_writeIndex;

      char _padding2[platform::CacheLineSize];

      unique_ptr<Entry[]> m_array;
      unsigned m_capacity;
    };
  }


  template<typename T>
  class SpscUnalignedValueQueue
  {
  public:

    explicit SpscUnalignedValueQueue(unsigned capacity)
     : m_readIndex(0)
     , m_writeIndex(0)
     , m_array(new Entry[capacity])
     , m_capacity(capacity)
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");
    }

    ~SpscUnalignedValueQueue()
    {
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

    char _padding0[platform::CacheLineSize];

    unsigned m_readIndex;

    char _padding1[platform::CacheLineSize];

    unsigned m_writeIndex;

    char _padding2[platform::CacheLineSize];

    unique_ptr<Entry[]> m_array;
    unsigned m_capacity;
  };



  template<typename T>
  class SpscStorageValueQueue
  {
  public:

    explicit SpscStorageValueQueue(unsigned capacity)
     : m_readIndex(0)
     , m_writeIndex(0)
     , m_array(new Entry[capacity])
     , m_capacity(capacity)
    {
      assert(m_capacity >= 2 && "queue capacity must be at least 2");
    }

    ~SpscStorageValueQueue()
    {
    }

    bool write(T&& t)
    {
      auto& entry = m_array[m_writeIndex];

      if (!entry.hasValue.load(std::memory_order_acquire))
      {
        new (&entry.data) T(std::move(t));
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
        auto ptr = reinterpret_cast<T*>(&entry.data);
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
        return reinterpret_cast<T*>(&entry.data);
      }

      return nullptr;
    }

    void popFront()
    {
      auto& entry = m_array[m_readIndex];

      assert(entry.hasValue.load(std::memory_order_acquire));

      reinterpret_cast<T*>(&entry.data)->~T();
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
      std::aligned_storage<sizeof(T), alignof(T)> data;
    }; 

    char _padding0[platform::CacheLineSize];

    unsigned m_readIndex;

    char _padding1[platform::CacheLineSize];

    unsigned m_writeIndex;

    char _padding2[platform::CacheLineSize];

    unique_ptr<Entry[]> m_array;
    unsigned m_capacity;
  };



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

}

TEETIME_WARNING_POP