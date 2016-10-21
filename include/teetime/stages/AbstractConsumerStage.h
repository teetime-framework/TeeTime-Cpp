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
#include "../common.h"
#include "../platform.h"
#include "../ports/InputPort.h"
#include "../Runnable.h"
#include "../Optional.h"
 #include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class AbstractConsumerStage : public AbstractStage
  {
  public:
    explicit AbstractConsumerStage(const char* debugName = nullptr)
      : AbstractStage(debugName)
      , m_inputport(addNewInputPort<T>())
    {
      assert(m_inputport);
    }

    InputPort<T>& getInputPort()
    {
      assert(m_inputport);
      return *m_inputport;
    }   

  private:
    InputPort<T>* m_inputport;

    virtual void execute(T&& value) = 0;

    virtual void execute() override final
    {
      assert(m_inputport);

      //TEETIME_DEBUG() << "'execute' stage";
      auto v = m_inputport->receive();
      if(v) 
      {
        execute(std::move(*v));
      }
      else if(m_inputport->isClosed())
      {
        terminate();
      }
      else
      {
        std::this_thread::yield();
      }
    }

    virtual unique_ptr<Runnable> createRunnable() override final
    {
      return unique_ptr<Runnable>(new ConsumerStageRunnable(this));
    }
  };
}


