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
namespace internal
{
#if defined(__GNUC__) && __GNUC__ < 5
  //workaround for missing type traits in old compilers ;(
  template<typename T>
  struct trivial_copy
  {
    static const bool value = __has_trivial_copy(T) && __has_trivial_destructor(T);
  };
#else
  template<typename T>
  using trivial_copy = std::is_trivially_copyable<T>;
#endif
  template<bool trivial>
  struct helper;

  //make sure workaround works for some basic examples.
  static_assert(trivial_copy<int>::value, "");
  static_assert(trivial_copy<void*>::value, "");
  static_assert(!trivial_copy<shared_ptr<int>>::value, "");
  static_assert(!trivial_copy<unique_ptr<int>>::value, "");

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
}

template<typename T>
class SpscValueQueue
{
public:

  explicit SpscValueQueue(size_t capacity)
    : m_readIndex(0)
    , m_writeIndex(0)
    , m_array(new Entry[capacity])
    , m_capacity(capacity)
  {
    assert(m_capacity >= 2 && "queue capacity must be at least 2");
  }

  ~SpscValueQueue()
  {
  }

  bool write(T&& t)
  {
    const auto index = m_writeIndex;
    auto& entry = m_array[index];

    if (!entry.hasValue.load(std::memory_order_acquire))
    {
      internal::constructAt<T>(std::move(t), entry.ptr());
      entry.hasValue.store(true, std::memory_order_release);
      const auto next = index + 1;
      m_writeIndex = (next != m_capacity) ? next : 0;
      return true;
    }

    return false;
  }

  bool write(const T& t)
  {
    return write(std::move(T(t)));
  }

  bool read(T& value)
  {
    const auto index = m_readIndex;
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
    const auto index = m_readIndex;
    auto& entry = m_array[index];

    assert(entry.hasValue.load(std::memory_order_acquire));

    entry.ptr()->~T();
    entry.hasValue.store(false, std::memory_order_release);

    const auto& next = index + 1;
    m_readIndex = (next != m_capacity) ? next : 0;
  }

  size_t sizeGuess() const
  {
    const auto read = m_readIndex;
    const auto write = m_writeIndex;

    if (read > write) {
      return (m_capacity - read) + write;
    }

    return write - read;
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

  size_t m_readIndex;

  char _padding1[platform::CacheLineSize];

  size_t m_writeIndex;

  char _padding2[platform::CacheLineSize];

  const unique_ptr<Entry[]> m_array;
  const size_t m_capacity;
};


}

TEETIME_WARNING_POP