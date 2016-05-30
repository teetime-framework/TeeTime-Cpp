#pragma once
#include "common.h"

namespace teetime
{
  class AbstractStage
  {
  public:
    AbstractStage()
    {      
    }

    virtual ~AbstractStage();

    void executeStage();

  private:
    virtual void execute() = 0;
  };
}


