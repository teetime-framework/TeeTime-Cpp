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
#include <vector>

namespace teetime
{
  //forward decls.
  class Runnable;
  class AbstractInputPort;
  class AbstractOutputPort;
  template<typename T> class InputPort;
  template<typename T> class OutputPort;
  struct Signal;

  //Stage state.
  //TODO(johl): do we really need all those?
  enum class StageState
  {
    Created,
    Started,
    Terminating,
    Terminated
  };

  /**
   * Abstract base class for all stages.
   */
  class AbstractStage : public std::enable_shared_from_this<AbstractStage>
  {
  public:
    explicit AbstractStage(const char* debugName = nullptr);
    virtual ~AbstractStage();

    /**
     * Create a runnable to execute this stage in it's own thread.
     */
    virtual unique_ptr<Runnable> createRunnable() = 0;
    void executeStage();

    StageState currentState() const;
    void setState(StageState state);

    void onSignal(const Signal& signal);

    uint32 numInputPorts() const;
    uint32 numOutputPorts() const;
    AbstractInputPort* getInputPort(uint32 index);
    AbstractOutputPort* getOutputPort(uint32 index);

    const AbstractInputPort* getInputPort(uint32 index) const;
    const AbstractOutputPort* getOutputPort(uint32 index) const;
    const char* debugName() const;

  protected:
    /**
     * Create a typed input port.
     * @return (Non-owning) pointer to input port (don't delete this port manually, the AbstractStage takes care of that)
     */
    template<typename T>
    InputPort<T>* addNewInputPort();

    /**
     * Create a typed output port.
     * @return (Non-owning) pointer to input port (don't delete this port manually, the AbstractStage takes care of that)
     */
    template<typename T>
    OutputPort<T>* addNewOutputPort();

    /**
     * Get a reference to the list of (untyped) output ports. You can not modify or copy that list because
     * all ports are managed by AbstractStage.
     */
    const std::vector<unique_ptr<AbstractOutputPort>>& getOutputPorts() const;

    /**
     * Get a reference to the list of (untyped) input ports. You can not modify or copy that list because
     * all ports are managed by AbstractStage.
     */
    const std::vector<unique_ptr<AbstractInputPort>>& getInputPorts() const;

    void terminate();

  private:
    /**
     * actually execute the stage.
     */
    virtual void execute() = 0;

    template<typename T>
    using pointers = std::vector<unique_ptr<T>>;

    //current state
    StageState                   m_state;
    //all input ports
    pointers<AbstractInputPort>  m_inputPorts;
    //all output ports
    pointers<AbstractOutputPort> m_outputPorts;
    //debug name
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

  inline const AbstractInputPort* AbstractStage::getInputPort(uint32 index) const
  {
    assert(index < m_inputPorts.size());
    return m_inputPorts[index].get();
  }

  inline const AbstractOutputPort* AbstractStage::getOutputPort(uint32 index) const
  {
    assert(index < m_outputPorts.size());
    return m_outputPorts[index].get();
  }

  inline const char* AbstractStage::debugName() const
  {
    return m_debugName.c_str();
  }
}


