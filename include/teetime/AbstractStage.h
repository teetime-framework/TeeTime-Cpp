#pragma once
#include "common.h"
#include "Runnable.h"
#include <vector>

namespace teetime
{
  class Thread;
  class Signal;
  class AbstractInputPort;
  class AbstractOutputPort;

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


    std::vector<AbstractInputPort*>::iterator getInputPortsBegin()
    {
      return m_inputPorts.begin();
    }

    std::vector<AbstractInputPort*>::iterator getInputPortsEnd()
    {
      return m_inputPorts.end();
    }

    std::vector<AbstractOutputPort*>::iterator getOutputPortsBegin()
    {
      return m_outputPorts.begin();
    }

    std::vector<AbstractOutputPort*>::iterator getOutputPortsEnd()
    {
      return m_outputPorts.end();
    }    
  
    void registerPort(AbstractInputPort* port)
    {
      m_inputPorts.push_back(port);
    }

    void registerPort(AbstractOutputPort* port)
    {
      m_outputPorts.push_back(port);
    }

    const char* getDebugName() const
    {
      return m_debugName.c_str();
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


