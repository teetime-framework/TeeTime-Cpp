#pragma once
#include "AbstractStage.h"
#include "InputPort.h"

namespace teetime
{
  template<typename T>
  class AbstractConsumerStage : public AbstractStage
  {
  public:
    AbstractConsumerStage()
      : m_inputport(this)
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
      T t = m_inputport.receive();
      execute(std::move(t));
    }
  };
}


