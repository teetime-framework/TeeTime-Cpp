#pragma once
#include "Pipe.h"
#include "AbstractStage.h"
#include "BlockingQueue.h"
#include <mutex>
#include <exception>
#include <stdexcept>
#include <atomic>
#include "logging.h"

namespace teetime
{
  template<typename T>
  class SynchedPipe final : public Pipe<T>
  {
  public:
    SynchedPipe()
     : m_size(0)
    {      
    }

    explicit SynchedPipe(uint32 initialCapacity)
     : m_size(0)
    {
      m_buffer.reserve(initialCapacity);
    }

    virtual Optional<T> removeLast() override
    {
      Optional<T> ret;
      
      std::lock_guard<std::mutex> lock(m_mutex);
      if(m_size.load() > 0)
      {
        ret.set(std::move(m_buffer.back()));
        m_buffer.pop_back();
        m_size.fetch_sub(1);
      }
      
      return ret;
    }

    virtual void add(const T& t) override
    {
      std::lock_guard<std::mutex> lock(m_mutex);

      m_buffer.insert(m_buffer.begin(), t);
      m_size.fetch_add(1);
    }

    virtual void addSignal(const Signal& signal) override
    {    
      std::lock_guard<std::mutex> lock(m_mutex);
      m_signals.add(signal);
    }

    virtual void waitForStartSignal() override
    {      
      auto s = m_signals.take();
      if(s.type != SignalType::Start)
      {
        throw std::runtime_error("Wrong signal type");
      }
    }

    unsigned size() const
    {
      std::lock_guard<std::mutex> lock(m_mutex);
      return m_size.load();
    }      

  private:
    //TODO(johl): merge m_signals and m_buffer into one queue, so order is always preserved
    BlockingQueue<Signal> m_signals;
    std::vector<T> m_buffer;
    std::atomic<unsigned> m_size;
    mutable std::mutex m_mutex;
  };
}