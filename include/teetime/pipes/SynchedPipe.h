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
#include <mutex>
#include <exception>
#include <stdexcept>
#include <atomic>
#include "Pipe.h"
#include "../stages/AbstractStage.h"
#include "../BlockingQueue.h"
#include "../logging.h"
#include "../Signal.h"
#include "../Optional.h"

 #include "ProducerConsumerQueue.h"

namespace teetime
{  
#if 1
  template<typename T>
  class SynchedPipe final : public Pipe<T>
  {
  public:
    explicit SynchedPipe(uint32 initialCapacity)
     : m_queue(initialCapacity)
    {
    }

    virtual Optional<T> removeLast() override
    {
      T ret;
      if(m_queue.read(ret)) {
        return Optional<T>(ret);
      }
            
      return Optional<T>();
    }

    virtual void add(T&& t) override
    {
      while(true) {
        if(m_queue.write(std::move(t)))
          break;
      }      
    }

    virtual void addSignal(const Signal& signal) override
    {    
      if(signal.type == SignalType::Terminating)
      {
        this->close();
        return;
      }

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
      return static_cast<unsigned>(m_queue.sizeGuess());
    }      

    virtual bool isEmpty() const override
    {
      return (size() == 0);
    }

  private:
    //TODO(johl): merge m_signals and m_buffer into one queue, so order is always preserved?
    BlockingQueue<Signal> m_signals;
    folly::ProducerConsumerQueue<T> m_queue;
  };

#else  
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

    virtual void add(T&& t) override
    {
      std::lock_guard<std::mutex> lock(m_mutex);

      m_buffer.insert(m_buffer.begin(), std::move(t));
      m_size.fetch_add(1);
    }

    virtual void addSignal(const Signal& signal) override
    {    
      if(signal.type == SignalType::Terminating)
      {
        this->close();
        return;
      }

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

    virtual bool isEmpty() const override
    {
      return (size() == 0);
    }

  private:
    //TODO(johl): merge m_signals and m_buffer into one queue, so order is always preserved?
    BlockingQueue<Signal> m_signals;
    std::vector<T> m_buffer;
    std::atomic<unsigned> m_size;
    mutable std::mutex m_mutex;
  };
#endif  
}