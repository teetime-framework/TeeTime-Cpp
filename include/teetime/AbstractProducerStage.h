#pragma once
#include "AbstractStage.h"
#include "OutputPort.h"

namespace teetime
{
  template<typename T>
  class AbstractProducerStage : public AbstractStage
  {
  public:
    AbstractProducerStage()
    {
    }

    OutputPort<T>& getOutputPort()
    {
      return m_outputport;
    }

  private:
    OutputPort<T> m_outputport;

    virtual void execute() override = 0;
  };
}


