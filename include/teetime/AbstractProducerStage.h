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
     , m_outputport(addNewOutputPort<T>())
    {
      assert(m_outputport);
    }

    OutputPort<T>& getOutputPort()
    {
      assert(m_outputport);
      return *m_outputport;
    }

  private:
    OutputPort<T>* m_outputport;

    virtual unique_ptr<Runnable> createRunnable() override final
    {
      return unique_ptr<Runnable>(new ProducerStageRunnable(this));
    }

    virtual void execute() override = 0;
  };
}


