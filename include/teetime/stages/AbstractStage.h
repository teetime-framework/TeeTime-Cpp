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
#include <vector>

namespace teetime
{
  class Runnable;
  class AbstractInputPort;
  class AbstractOutputPort;
  template<typename T> class InputPort;
  template<typename T> class OutputPort;
  struct Signal;

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

    Runnable* getRunnable() const;

    StageState currentState() const;
    void setState(StageState state);

    void declareActive(int cpu = -1);
    void declareNonActive();

    void onSignal(const Signal& signal);

    uint32 numInputPorts() const;
    uint32 numOutputPorts() const;
    AbstractInputPort* getInputPort(uint32 index);
    AbstractOutputPort* getOutputPort(uint32 index);
    const char* debugName() const;

  protected:   
    template<typename T>
    InputPort<T>* addNewInputPort();

    template<typename T>
    OutputPort<T>* addNewOutputPort();

    void terminate();

    const std::vector<unique_ptr<AbstractOutputPort>>& getOutputPorts() const;
    const std::vector<unique_ptr<AbstractInputPort>>& getInputPorts() const;

  private:   
    virtual unique_ptr<Runnable> createRunnable(int cpu) = 0;

    virtual void execute() = 0;

    template<typename T>
    using pointers = std::vector<unique_ptr<T>>;

    StageState                   m_state;
    unique_ptr<Runnable>         m_runnable;
    pointers<AbstractInputPort>  m_inputPorts;
    pointers<AbstractOutputPort> m_outputPorts;
    std::string                  m_debugName;
  };

  template<typename T>
  InputPort<T>* AbstractStage::addNewInputPort()
  {
    if(m_state != StageState::Created)
    {
      throw std::logic_error("cannot create new ports after stage has been started");
    }

    InputPort<T>* port = new InputPort<T>(this);
    m_inputPorts.push_back(unique_ptr<AbstractInputPort>(port));
    return port;
  }

  template<typename T>
  OutputPort<T>* AbstractStage::addNewOutputPort()
  {
    if(m_state != StageState::Created)
    {
      throw std::logic_error("cannot create new ports after stage has been started");
    }
        
    OutputPort<T>* port = new OutputPort<T>(this);
    m_outputPorts.push_back(unique_ptr<AbstractOutputPort>(port));
    return port;
  }   

  inline const std::vector<unique_ptr<AbstractOutputPort>>& AbstractStage::getOutputPorts() const
  {
    return m_outputPorts;
  }

  inline const std::vector<unique_ptr<AbstractInputPort>>& AbstractStage::getInputPorts() const
  {
    return m_inputPorts;
  }

  inline StageState AbstractStage::currentState() const
  {
    return m_state;
  }

  inline void AbstractStage::setState(StageState state)
  {
    m_state = state;
  }

  inline uint32 AbstractStage::numInputPorts() const
  {
    size_t s = m_inputPorts.size();
    assert(s < UINT32_MAX);

    return static_cast<uint32>(s);
  }

  inline uint32 AbstractStage::numOutputPorts() const
  {
    size_t s = m_outputPorts.size();
    assert(s < UINT32_MAX);

    return static_cast<uint32>(s);
  }

  inline AbstractInputPort* AbstractStage::getInputPort(uint32 index)
  {
    assert(index < m_inputPorts.size());
    return m_inputPorts[index].get();
  }

  inline AbstractOutputPort* AbstractStage::getOutputPort(uint32 index)
  {
    assert(index < m_outputPorts.size());
    return m_outputPorts[index].get();
  }

  inline const char* AbstractStage::debugName() const
  {
    return m_debugName.c_str();
  }
}


