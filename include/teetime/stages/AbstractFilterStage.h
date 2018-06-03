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
#include "AbstractConsumerStage.h"
#include "../ports/OutputPort.h"

namespace teetime
{
  template<typename TIn, typename TOut = TIn>
  class AbstractFilterStage : public AbstractConsumerStage<TIn>
  {
  public:
    explicit AbstractFilterStage(const char* debugName = nullptr)
      : AbstractConsumerStage<TIn>(debugName)
      , m_outputport(AbstractStage::addNewOutputPort<TOut>())
    {
      assert(m_outputport);
    }

    OutputPort<TOut>& getOutputPort()
    {
      assert(m_outputport);
      return *m_outputport;
    }

  private:
    OutputPort<TOut>* m_outputport;

    virtual void execute(TIn&& value) = 0;
  };
}


