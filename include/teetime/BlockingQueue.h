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
#include <vector>
#include <mutex>
#include <condition_variable>

namespace teetime
{
  /**
   * Simple queue of values.
   * Access is threadsafe but blocks.
   * @tparam T type of values
   */
  template<typename T>
  class BlockingQueue
  {
  public:
    BlockingQueue() = default;
    ~BlockingQueue() = default;

    BlockingQueue(const BlockingQueue&) = delete;
    BlockingQueue& operator=(const BlockingQueue&) = delete;

    void add(const T& t)
    {
      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push_back(t);        
      }

      m_cond.notify_one();
    }

    T take()
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      while(m_queue.empty())
      {
        m_cond.wait(lock);
      }

      assert(m_queue.size() > 0);
      T ret = std::move(m_queue[0]);
      m_queue.erase(m_queue.begin());
      return ret;
    }

    size_t size() const 
    {
      return m_queue.size();
    }

    size_t capacity() const
    {
      return m_queue.capacity();
    }

    void reserve(size_t n)
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      m_queue.reserve(n);
    }

  private:
    std::mutex              m_mutex;
    std::vector<T>          m_queue;
    std::condition_variable m_cond;
  };
}