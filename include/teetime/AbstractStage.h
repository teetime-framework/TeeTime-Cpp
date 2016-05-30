#pragma once
#include "common.h"

namespace teetime
{
  class Thread;

  class AbstractStage
  {
  public:
    AbstractStage();
    virtual ~AbstractStage();

    void executeStage();

    Thread* getOwningThread() const
    {
      return m_thread.get();
    }

    void declareActive();
    void declareNonActive();

  private:
    virtual void execute() = 0;

    unique_ptr<Thread> m_thread;
  };
}


