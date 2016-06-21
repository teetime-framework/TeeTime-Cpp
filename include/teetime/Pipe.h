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
#include "Signal.h"
#include "Optional.h"
#include <atomic>

namespace teetime
{
  class AbstractPipe
  {
  public:
    AbstractPipe()
    : m_closed(false)
    {      
    }

    virtual ~AbstractPipe() = default;

    virtual void addSignal(const Signal& s) = 0;
    virtual void waitForStartSignal() = 0;

    bool isClosed() const
    {
      return m_closed;
    }

    void close()
    {
      m_closed = true;
    }

  private:
    std::atomic<bool> m_closed;
  };

  template<typename T>
  class Pipe : public AbstractPipe
  {
  public:
    virtual ~Pipe() = default;
    virtual Optional<T> removeLast() = 0;
    virtual void add(const T& t) = 0;
    virtual bool isEmpty() const = 0;
  };
}