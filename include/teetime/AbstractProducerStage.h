#pragma once
#include "AbstractStage.h"
#include "OutputPort.h"

namespace teetime
{
  template<typename T>
  class AbstractProducerStage : public AbstractStage
  {
  public:
    explicit AbstractProducerStage(const char* debugName = nullptr)
     : AbstractStage(debugName)
     , m_outputport(this)
    {
    }

    OutputPort<T>& getOutputPort()
    {
      return m_outputport;
    }

  private:
    OutputPort<T> m_outputport;

    virtual unique_ptr<Runnable> createRunnable()
    {
      return unique_ptr<Runnable>(new ProducerStageRunnable(this));
    }

    virtual void execute() override = 0;
  };
}


