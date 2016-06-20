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
  TEETIME_INFO() << "Start producer stage '" << m_stage->getDebugName() << "'";
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

  TEETIME_INFO() << "Start consumer stage '" << m_stage->getDebugName() << "'";
  m_stage->executeStage();
}
