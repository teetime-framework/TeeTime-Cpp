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
#include "../platform.h"

#include "SpscQueue.h"

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_CONSTANT_CONDITION
#include "ProducerConsumerQueue.h"
#include "AlignedProducerConsumerQueue.h"
TEETIME_WARNING_POP



namespace teetime
{
  template<typename T, template<typename> class TQueue = folly::ProducerConsumerQueue>
  class SynchedPipe final : public Pipe<T>
  {
  public:
    explicit SynchedPipe(uint32 initialCapacity)
     : m_queue(initialCapacity)
    {
    }

    virtual Optional<T> removeLast() override
    {
      if (T* p = m_queue.frontPtr())
      {
        Optional<T> ret(std::move(*p));
        m_queue.popFront();

        return ret;
      }

      return Optional<T>();
    }

    virtual bool tryAdd(T&& t) override
    {
      return m_queue.write(std::move(t));
    }

    virtual void add(T&& t) override
    {
      while (!m_queue.write(std::move(t)))
      {
        std::this_thread::yield();
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

    void* operator new(size_t i)
    {
      return platform::aligned_malloc(i, 64);
    }

    void operator delete(void* p)
    {
      platform::aligned_free(p);
    }

  private:
    //TODO(johl): merge m_signals and m_buffer into one queue, so order is always preserved?
    BlockingQueue<Signal> m_signals;
    TQueue<T> m_queue;
  };
}