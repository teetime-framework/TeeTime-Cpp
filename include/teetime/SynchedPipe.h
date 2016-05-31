#pragma once
#include "Pipe.h"
#include "AbstractStage.h"
#include <mutex>

namespace teetime
{
  template<typename T>
  class SynchedPipe final : public Pipe<T>
  {
  public:
    explicit SynchedPipe(uint32 initialCapacity)
    {
      m_buffer.reserve(initialCapacity);
    }

    SynchedPipe()
    {
    }

    virtual T removeLast() override
    {
      std::lock_guard<std::mutex> lock(m_mutex);

      T t = std::move(m_buffer.back());
      m_buffer.pop_back();
      return t;
    }

    virtual void add(const T& t) override
    {
      std::lock_guard<std::mutex> lock(m_mutex);

      m_buffer.insert(m_buffer.begin(), t);
    }

    size_t size() const
    {
      return m_buffer.size();
    }

  private:
    std::vector<T> m_buffer;
    std::mutex m_mutex;
  };
}