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
  
    const char* getDebugName() const
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

  private:
    virtual unique_ptr<Runnable> createRunnable() = 0;
    virtual void execute() = 0;
    unique_ptr<Runnable> m_runnable;

    std::vector<AbstractInputPort*> m_inputPorts;
    std::vector<AbstractOutputPort*> m_outputPorts;
    std::string m_debugName;
  };
}


