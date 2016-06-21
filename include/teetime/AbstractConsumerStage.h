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
      , m_inputport(addNewInputPort<T>())
    {
      assert(m_inputport);
    }

    InputPort<T>& getInputPort()
    {
      assert(m_inputport);
      return *m_inputport;
    }   

  private:
    InputPort<T>* m_inputport;

    virtual void execute(const T& value) = 0;

    virtual void execute() override final
    {
      assert(m_inputport);

      //TEETIME_DEBUG() << "'execute' stage";
      auto v = m_inputport->receive();
      if(v) 
      {
        execute(std::move(*v));
      }
      else if(m_inputport->isClosed())
      {
        terminate();
      }
    }

    virtual unique_ptr<Runnable> createRunnable() override final
    {
      return unique_ptr<Runnable>(new ConsumerStageRunnable(this));
    }
  };
}


