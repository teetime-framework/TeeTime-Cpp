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
#include <teetime/platform.h>
#include <teetime/stages/AbstractStage.h>
#include <teetime/ports/InputPort.h>
#include <teetime/ports/OutputPort.h>

using namespace teetime;
  
AbstractStageRunnable::AbstractStageRunnable(AbstractStage* stage, int cpu)
 : m_stage(stage)
 , m_cpu(cpu)
{
  assert(m_stage);
}

ProducerStageRunnable::ProducerStageRunnable(AbstractStage* stage, int cpu)
 : AbstractStageRunnable(stage, cpu)
{

}

void ProducerStageRunnable::run()
{
  TEETIME_INFO() << "ProducerStageRunnable::run(): " << m_stage->debugName();

  const uint32 numOutputPorts = m_stage->numOutputPorts();
  for(uint32 i=0; i<numOutputPorts; ++i)
  {
    TEETIME_DEBUG() << "send start signal";
    auto port = m_stage->getOutputPort(i);
    assert(port);
    port->sendSignal(Signal{SignalType::Start, m_stage});
  }

  TEETIME_DEBUG() << "execute producer stage '" << m_stage->debugName() << "'";
  m_stage->executeStage();      
}

ConsumerStageRunnable::ConsumerStageRunnable(AbstractStage* stage, int cpu)
 : AbstractStageRunnable(stage, cpu)
{
}

void ConsumerStageRunnable::run()
{
  TEETIME_INFO() << "ConsumerStageRunnable::run(): " << m_stage->debugName();

  if (m_cpu >= 0)
  {
    platform::setThreadAffinityMask(1 << m_cpu);
  }

  const uint32 numInputPorts = m_stage->numInputPorts();
  for(uint32 i=0; i<numInputPorts; ++i)
  {
    TEETIME_DEBUG() << "wait fors start signal";    
    auto port = m_stage->getInputPort(i);
    assert(port);
    port->waitForStartSignal();
  }

  m_stage->setState(StageState::Started);

  {
    const uint32 numOutputPorts = m_stage->numOutputPorts();
    for(uint32 i=0; i<numOutputPorts; ++i)
    {
      auto port = m_stage->getOutputPort(i);
      assert(port);
      port->sendSignal(Signal{ SignalType::Start, m_stage });
    }
  }

  TEETIME_DEBUG() << "execute consumer stage '" << m_stage->debugName() << "'";  
  while(m_stage->currentState() == StageState::Started)
  {
    m_stage->executeStage();
  }

  TEETIME_DEBUG() << "terminating consumer stage '" << m_stage->debugName() << "'";
  //FIXME(johl): should we assert the current state is 'Terminating'?
  //assert(m_stage->currentState() == StageState::Terminating);

  m_stage->setState(StageState::Terminated);

  {
    const uint32 numOutputPorts = m_stage->numOutputPorts();
    for(uint32 i=0; i<numOutputPorts; ++i)
    {
      auto port = m_stage->getOutputPort(i);
      assert(port);
      port->sendSignal(Signal{ SignalType::Terminating, m_stage });
    }
  }

  TEETIME_DEBUG() << "leaving stage '" << m_stage->debugName() << "'";
}
