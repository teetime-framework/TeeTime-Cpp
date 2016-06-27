/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include "InputPort.h"
#include "OutputPort.h"
#include "../pipes/UnsynchedPipe.h"
#include "../pipes/SynchedPipe.h"

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