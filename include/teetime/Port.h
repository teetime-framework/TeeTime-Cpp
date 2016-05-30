#pragma once

#include "InputPort.h"
#include "OutputPort.h"
#include "UnsynchedPipe.h"

namespace teetime
{
  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input)
  {
    output.m_pipe.reset(new UnsynchedPipe<T>(input.getOwningStage()));
    input.m_pipe = output.m_pipe.get();
  }
}