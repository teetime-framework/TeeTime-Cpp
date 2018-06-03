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
#include "Pipe.h"
#include "../stages/AbstractStage.h"
#include "../Optional.h"

namespace teetime
{
  /**
   * forwards input elements to target stage.
   */
  template<typename T>
  class UnsynchedPipe : public Pipe<T>
  {
  public:
    explicit UnsynchedPipe(AbstractStage* targetStage)
     : m_targetStage(targetStage)
    {
    }

    virtual Optional<T> removeLast() override
    {
      Optional<T> ret = std::move(m_value);
      m_value.reset();
      return ret;
    }

    virtual bool tryAdd(T&& t) override
    {
      add(std::move(t));
      return true;
    }

    virtual void add(T&& t) override
    {
      //TODO(johl): what to do if pipe is non-empty?
      assert(!m_value);
      m_value.set(std::move(t));

      m_targetStage->executeStage();
    }

    virtual void addSignal(const Signal& signal) override
    {
      if(signal.type == SignalType::Terminating)
      {
        this->close();
      }

      m_targetStage->onSignal(signal);
    }

    virtual void waitForStartSignal() override
    {
      //do nothing
    }

    virtual bool isEmpty() const override
    {
      return !m_value;
    }

  private:

    Optional<T> m_value;
    AbstractStage* m_targetStage;
  };
}