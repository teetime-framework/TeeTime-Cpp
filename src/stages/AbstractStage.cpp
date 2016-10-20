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
#include <teetime/stages/AbstractStage.h>
#include <exception>
#include <teetime/logging.h>
#include <teetime/Signal.h>
#include <teetime/Runnable.h>
#include <teetime/ports/AbstractInputPort.h>
#include <teetime/ports/AbstractOutputPort.h>

using namespace teetime;

AbstractStage::AbstractStage(const char* debugName)
  : m_state(StageState::Created)
{
  if(debugName) 
  {
    m_debugName = debugName;
  }
  else
  {
    char buffer[128];
    sprintf(buffer, "%p", (void*)this);

    m_debugName = buffer;
  }
}

AbstractStage::~AbstractStage()
{

}

void AbstractStage::executeStage()
{
  try
  {
    execute();
  }
  catch( const std::exception& e )
  {
    TEETIME_ERROR() << "stage '" << this->debugName() << "' execution failed: " << e.what();    
  }
  catch( ... )
  {
    TEETIME_ERROR() << "stage '" << this->debugName() << "' execution failed due to unknown error";
  }
}

Runnable* AbstractStage::getRunnable() const
{
  return m_runnable.get();
}

void AbstractStage::declareActive(int cpu)
{   
  m_runnable = createRunnable(cpu); 
}

void AbstractStage::declareNonActive()
{
  m_runnable.reset();
}

void AbstractStage::onSignal(const Signal& s)
{
  if (s.sender == this)
    return;

  if(s.type == SignalType::Terminating) 
  {
    TEETIME_DEBUG() << debugName() << ": Terminating signal received";
    terminate();
  }
  else
  {
    for (const auto& p : m_outputPorts)
    {
      p->sendSignal(s);
    }    
  }
}


void AbstractStage::terminate()
{
  TEETIME_DEBUG() << debugName() << ": terminating stage...";
  //assert(m_state == StageState::Started);
  m_state = StageState::Terminating;

  for(const auto& p : m_outputPorts)
  {
    TEETIME_DEBUG() << debugName() << " : send Terminating signal";
    p->sendSignal(Signal{SignalType::Terminating, this});
  }

  m_state = StageState::Terminated;
}