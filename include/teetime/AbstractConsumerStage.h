#pragma once
#include "AbstractStage.h"
#include "InputPort.h"

namespace teetime
{
  template<typename T>
  class AbstractConsumerStage : public AbstractStage
  {
  public:
    explicit AbstractConsumerStage(const char* debugName = nullptr)
      : AbstractStage(debugName)
      , m_inputport(this)
    {
    }

    InputPort<T>& getInputPort()
    {
      return m_inputport;
    } 

  private:
    InputPort<T> m_inputport;

    virtual void execute(const T& value) = 0;

    virtual void execute() override
    {
      auto v = m_inputport.receive();
      if(v) 
      {
        execute(std::move(*v));
      }
      
    }

    virtual unique_ptr<Runnable> createRunnable()
    {
      return unique_ptr<Runnable>(new ConsumerStageRunnable(this));
    }
  };
}


