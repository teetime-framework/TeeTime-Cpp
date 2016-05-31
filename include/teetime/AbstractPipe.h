#if 0

#pragma once
#include "Pipe.h"
#include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class AbstractPipe : public Pipe<T>
  {
  public:

  protected:
    AbstractPipe(AbstractStage* targetStage)
      : m_targetStage(targetStage)
    {
    }

    void notifyTargetStage()
    {
      m_targetStage->executeStage();
    }

  private:
    AbstractStage* m_targetStage;
  };

}

#endif