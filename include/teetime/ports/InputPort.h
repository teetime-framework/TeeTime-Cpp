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
#include "AbstractInputPort.h"
#include "../pipes/Pipe.h"

namespace teetime
{
  template<typename T>
  class InputPort;

  template<typename T>  
  class OutputPort;

  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input);

  class AbstractStage;

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
    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    Pipe<T>* m_pipe;
  };
}