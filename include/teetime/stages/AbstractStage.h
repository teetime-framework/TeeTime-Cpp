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

    void declareActive();
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

  private:
    StageState m_state;

    virtual unique_ptr<Runnable> createRunnable() = 0;
    virtual void execute() = 0;
    unique_ptr<Runnable> m_runnable;

    std::vector<AbstractInputPort*> m_inputPorts;
    std::vector<AbstractOutputPort*> m_outputPorts;
    std::string m_debugName;
  };

  template<typename T>
  InputPort<T>* AbstractStage::addNewInputPort()
  {
    InputPort<T>* port = new InputPort<T>(this);
    m_inputPorts.push_back(port);
    return port;
  }

  template<typename T>
  OutputPort<T>* AbstractStage::addNewOutputPort()
  {
    OutputPort<T>* port = new OutputPort<T>(this);
    m_outputPorts.push_back(port);
    return port;
  }   
}


