#pragma once
#include "teetime/AbstractPort.h"

namespace teetime
{
  class AbstractInputPort : public AbstractPort
  {
  public:
    explicit AbstractInputPort(AbstractStage* owner)
     : AbstractPort(owner)
    {}

    virtual ~AbstractInputPort() = default;

    virtual void waitForStartSignal() = 0;

    virtual bool isClosed() const = 0;
  };
}