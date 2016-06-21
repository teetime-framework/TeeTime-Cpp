#pragma once
#include "teetime/common.h"
#include "teetime/AbstractPort.h"

namespace teetime
{
  struct Signal;
  class AbstractPipe;

  class AbstractOutputPort : public AbstractPort
  {
  public:
    explicit AbstractOutputPort(AbstractStage* owner)
     : AbstractPort(owner)
    {   
    }

    void sendSignal(const Signal& signal);

  private:
    virtual AbstractPipe* getPipe() = 0;

  };
}