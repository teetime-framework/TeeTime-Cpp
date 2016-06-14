#pragma once
#include "Pipe.h"
#include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class UnsynchedPipe : public Pipe<T>
  {
  public:
    explicit UnsynchedPipe(AbstractStage* targetStage)
     : m_targetStage(targetStage)
    {      
    }

    virtual Optional<T> removeLast() override
    {
      Optional<T> ret = std::move(m_value);
      m_value.reset();
      return ret;
    }

    virtual void add(const T& t) override
    {
      //TODO(johl): what to do if pipe is non-empty?
      assert(!m_value);
      m_value.set(t);

      m_targetStage->executeStage();
    }


    virtual void addSignal(const Signal& signal) override
    {    
      m_targetStage->onSignal(signal);
    }

    virtual void waitForStartSignal() override
    {
      //do nothing
    }

  private:

    Optional<T> m_value;
    AbstractStage* m_targetStage;
  };
}