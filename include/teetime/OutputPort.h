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
#include "Pipe.h"
#include "Signal.h"

namespace teetime
{
  template<typename T>
  class InputPort;

  template<typename T>  
  class OutputPort;

  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input);
  
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

    void send(const T& t) {
      if(m_pipe) {
        m_pipe->add(t);
      }      
    }

  private:  
    virtual AbstractPipe* getPipe() override
    {
      return m_pipe.get();
    }

    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    unique_ptr<Pipe<T>> m_pipe;
  };
}