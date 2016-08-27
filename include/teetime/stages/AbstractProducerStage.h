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
#include "../Runnable.h"
#include "../ports/OutputPort.h"
#include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class AbstractProducerStage : public AbstractStage
  {
  public:
    explicit AbstractProducerStage(const char* debugName = nullptr)
     : AbstractStage(debugName)
     , m_outputport(addNewOutputPort<T>())
    {
      assert(m_outputport);
    }

    OutputPort<T>& getOutputPort()
    {
      assert(m_outputport);
      return *m_outputport;
    }

  private:
    OutputPort<T>* m_outputport;

    virtual unique_ptr<Runnable> createRunnable(int cpu) override final
    {
      return unique_ptr<Runnable>(new ProducerStageRunnable(this, cpu));
    }

    virtual void execute() override = 0;
  };
}


