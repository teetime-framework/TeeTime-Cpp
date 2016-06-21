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
#include <teetime/Runnable.h>
#include <teetime/logging.h>
#include <teetime/AbstractStage.h>
#include <teetime/InputPort.h>
#include <teetime/OutputPort.h>

using namespace teetime;
  
AbstractStageRunnable::AbstractStageRunnable(AbstractStage* stage)
 : m_stage(stage)
{
  assert(m_stage);
}

ProducerStageRunnable::ProducerStageRunnable(AbstractStage* stage)
 : AbstractStageRunnable(stage)
{

}

void ProducerStageRunnable::run()
{
  const uint32 numOutputPorts = m_stage->numOutputPorts();
  for(uint32 i=0; i<numOutputPorts; ++i)
  {
    TEETIME_INFO() << "send start signal";
    auto port = m_stage->getOutputPort(i);
    assert(port);
    port->sendSignal(Signal{SignalType::Start});
  }
  TEETIME_INFO() << "Start producer stage '" << m_stage->debugName() << "'";
  m_stage->executeStage();      
}

ConsumerStageRunnable::ConsumerStageRunnable(AbstractStage* stage)
 : AbstractStageRunnable(stage)
{

}

void ConsumerStageRunnable::run()
{
  const uint32 numInputPorts = m_stage->numInputPorts();
  for(uint32 i=0; i<numInputPorts; ++i)
  {
    TEETIME_INFO() << "wait fors start signal";    
    auto port = m_stage->getInputPort(i);
    assert(port);
    port->waitForStartSignal();
  }

  m_stage->setState(StageState::Started);

  TEETIME_INFO() << "Start consumer stage '" << m_stage->debugName() << "'";  
  while(m_stage->currentState() == StageState::Started)
  {
    m_stage->executeStage();
  }

  TEETIME_INFO() << "Terminating consumer stage '" << m_stage->debugName() << "'";
  assert(m_stage->currentState() == StageState::Terminating);

  m_stage->setState(StageState::Terminated);

  const uint32 numOutputPorts = m_stage->numOutputPorts();
  for(uint32 i=0; i<numOutputPorts; ++i)
  {
    auto port = m_stage->getOutputPort(i);
    assert(port);
    port->sendSignal(Signal{SignalType::Terminating});
  }

  TEETIME_INFO() << "Leaving stage '" << m_stage->debugName() << "'";
}
