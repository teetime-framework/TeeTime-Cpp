#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

namespace teetime
{
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