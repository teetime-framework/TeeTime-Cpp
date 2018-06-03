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
#include "AbstractOutputPort.h"
#include "../pipes/Pipe.h"
#include "../Signal.h"

namespace teetime
{
  //forward decls.
  class AbstractInputPort;

  template<typename T>
  class InputPort;

  template<typename T>
  class OutputPort;

  namespace internal
  {
    template<typename T>
    void connectPortsCallback(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched);
  }

  /**
   * Output port.
   * @tparam T type of data elements, that can be sent through this port.
   */
  template<typename T>
  class OutputPort : public AbstractOutputPort
  {
  public:
    explicit OutputPort(AbstractStage* owner)
    : AbstractOutputPort(owner)
    , m_pipe(nullptr)
    {

    }

    OutputPort(const OutputPort&) = delete;

    void send(T&& t) {
      assert(m_pipe);
      m_pipe->add(std::move(t));
    }

    bool trySend(T&& t)
    {
      assert(m_pipe);
      return m_pipe->tryAdd(std::move(t));
    }

  private:
    virtual AbstractPipe* getPipe() override
    {
      return m_pipe.get();
    }

    friend void internal::connectPortsCallback<T>(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched);

    unique_ptr<Pipe<T>> m_pipe;
  };
}