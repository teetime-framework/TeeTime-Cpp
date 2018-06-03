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
#include "../common.h"
#include "../ports/InputPort.h"
#include "../Runnable.h"
#include "../Optional.h"
#include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class MergerStage final : public AbstractStage
  {
  public:
    explicit MergerStage(const char* debugName = "MergerStage")
      : AbstractStage(debugName)
    {
      m_outputPort = this->addNewOutputPort<T>();
      assert(m_outputPort);
    }

    OutputPort<T>& getOutputPort()
    {
      assert(m_outputPort);
      return *m_outputPort;
    }

    InputPort<T>& getNewInputPort()
    {
      InputPort<T>* p = this->addNewInputPort<T>();
      assert(p);
      return *p;
    }

  private:
    OutputPort<T>* m_outputPort;

    virtual void execute() override final
    {
      const uint32 numInputPorts = this->numInputPorts();

      uint32 closedPorts = 0;

      for(uint32 i = 0; i < numInputPorts; ++i)
      {
        auto abstractPort = getInputPort(i);
        auto typedPort = unsafe_dynamic_cast<InputPort<T>>(abstractPort);

        auto v = typedPort->receive();
        if(v)
        {
          m_outputPort->send(std::move(*v));
        }
        else if(typedPort->isClosed())
        {
          closedPorts += 1;
        }
      }

      if(closedPorts == numInputPorts)
      {
        terminate();
      }
    }

    virtual unique_ptr<Runnable> createRunnable() override final
    {
      return unique_ptr<Runnable>(new ConsumerStageRunnable(this));
    }
  };
}


