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
#include "common.h"
#include "Runnable.h"
#include "InputPort.h"
#include "OutputPort.h"
#include <vector>

namespace teetime
{
  class Thread;
  class Signal;

  enum class StageState
  {
    Created,
    Started,
    Terminating,
    Terminated
  };

  class AbstractStage
  {
  public:
    explicit AbstractStage(const char* debugName = nullptr);
    virtual ~AbstractStage();

    void executeStage();

    Runnable* getRunnable() const
    {
      return m_runnable.get();
    }

    StageState currentState() const;
    void setState(StageState state);

    void declareActive();
    void declareNonActive();

    void onSignal(const Signal& signal);

    uint32 numInputPorts() const
    {
      size_t s = m_inputPorts.size();
      assert(s < UINT32_MAX);

      return static_cast<uint32>(s);
    }

    uint32 numOutputPorts() const
    {
      size_t s = m_outputPorts.size();
      assert(s < UINT32_MAX);
      
      return static_cast<uint32>(s);
    }

    AbstractInputPort* getInputPort(uint32 index)
    {
      assert(index < m_inputPorts.size());
      return m_inputPorts[index];
    }

    AbstractOutputPort* getOutputPort(uint32 index)
    {
      assert(index < m_outputPorts.size());
      return m_outputPorts[index];
    }
  
    const char* debugName() const
    {
      return m_debugName.c_str();
    }   

  protected:
    //TODO(johl): should we use some kind of pool of pre-allocated ports?
    
    template<typename T>
    InputPort<T>* addNewInputPort()
    {
      InputPort<T>* port = new InputPort<T>(this);
      m_inputPorts.push_back(port);
      return port;
    }

    template<typename T>
    OutputPort<T>* addNewOutputPort()
    {
      OutputPort<T>* port = new OutputPort<T>(this);
      m_outputPorts.push_back(port);
      return port;
    } 

    void terminate()
    {
      TEETIME_DEBUG() << debugName() << ": terminating stage...";
      //assert(m_state == StageState::Started);
      m_state = StageState::Terminating;

      for(auto p : m_outputPorts)
      {
        TEETIME_DEBUG() << debugName() << " : send Terminating signal";
        p->sendSignal(Signal{SignalType::Terminating});
      }
    }

  private:
    StageState m_state;

    virtual unique_ptr<Runnable> createRunnable() = 0;
    virtual void execute() = 0;
    unique_ptr<Runnable> m_runnable;

    std::vector<AbstractInputPort*> m_inputPorts;
    std::vector<AbstractOutputPort*> m_outputPorts;
    std::string m_debugName;
  };
}


