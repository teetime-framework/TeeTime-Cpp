#pragma once

#include "InputPort.h"
#include "OutputPort.h"
#include "UnsynchedPipe.h"
#include "SynchedPipe.h"

namespace teetime
{
  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input)
  {
    if (input.owner()->getRunnable())
    {
      output.m_pipe.reset(new SynchedPipe<T>(1024));
    }
    else
    {
      output.m_pipe.reset(new UnsynchedPipe<T>(input.owner()));
    }
    
    input.m_pipe = output.m_pipe.get();
  }
}