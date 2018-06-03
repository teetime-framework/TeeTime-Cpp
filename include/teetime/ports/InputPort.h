/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/teetime-framework/TeeTime-Cpp)
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
#include "AbstractInputPort.h"
#include "../pipes/Pipe.h"

namespace teetime
{
  //forward decls

  class AbstractOutputPort;

  template<typename T>
  class InputPort;

  template<typename T>
  class OutputPort;

  namespace internal
  {
    template<typename T>
    void connectPortsCallback(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched);
  }

  class AbstractStage;

  /**
   * Input port.
   * @tparam T type of data elements, that can be received through this port.
   */
  template<typename T>
  class InputPort final : public AbstractInputPort
  {
  public:
    explicit InputPort(AbstractStage* owner)
     : AbstractInputPort(owner)
    {
    }

    InputPort(const InputPort&) = delete;

    Optional<T> receive() {
      return m_pipe->removeLast();
    }

    virtual void waitForStartSignal() override
    {
      m_pipe->waitForStartSignal();
    }

    bool isClosed() const
    {
      return m_pipe->isClosed() && m_pipe->isEmpty();
    }

  private:
    friend void internal::connectPortsCallback<T>(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched);

    Pipe<T>* m_pipe;
  };
}