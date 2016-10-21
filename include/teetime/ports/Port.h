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

#ifndef TEETIME_DEFAULT_QUEUE
 //#define TEETIME_DEFAULT_QUEUE folly::ProducerConsumerQueue
 //#define TEETIME_DEFAULT_QUEUE folly::AlignedProducerConsumerQueue
#define TEETIME_DEFAULT_QUEUE teetime::SpscValueQueue
#endif

namespace teetime
{
  template<typename T>
  void connect2(OutputPort<T>& output, InputPort<T>& input, size_t capacity)
  {
    assert(output.owner());
    assert(input.owner());

    TEETIME_DEBUG() << "connecting '" << output.owner()->debugName() << "' to '" << input.owner()->debugName() << "'";
    if (input.owner()->getRunnable())
    {
      output.m_pipe.reset(new SynchedPipe<T, v2::SpscValueQueue>(capacity));
    }
    else
    {
      output.m_pipe.reset(new UnsynchedPipe<T>(input.owner()));
    }

    input.m_pipe = output.m_pipe.get();
  }

  template<typename T>
  void connect2(OutputPort<T>& output, InputPort<T>& input)
  {
 
    assert(output.owner());
    assert(input.owner());

    TEETIME_DEBUG() << "connecting '" << output.owner()->debugName() << "' to '" << input.owner()->debugName() << "'";
    if (input.owner()->getRunnable())
    {
      output.m_pipe.reset(new SynchedPipe<T, v2::SpscValueQueue>(1024));
    }
    else
    {
      output.m_pipe.reset(new UnsynchedPipe<T>(input.owner()));
    }

    input.m_pipe = output.m_pipe.get();  
  }

  template<typename T>
  void connectPortsCallback(AbstractOutputPort* out, AbstractInputPort* in, unsigned capacity, bool synched)
  {
    auto typed_out = unsafe_dynamic_cast<OutputPort<T>>(out);
    auto typed_in = unsafe_dynamic_cast<InputPort<T>>(in);

    if (synched)
    {
      typed_out->m_pipe.reset(new SynchedPipe<T, v2::SpscValueQueue>(capacity));
    }
    else
    {
      typed_out->m_pipe.reset(new UnsynchedPipe<T>(typed_in->owner()));
    }

    typed_in->m_pipe = typed_out->m_pipe.get();
  }
}