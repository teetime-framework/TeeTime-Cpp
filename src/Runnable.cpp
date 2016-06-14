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
  auto begin = m_stage->getOutputPortsBegin();
  auto end = m_stage->getOutputPortsEnd();
  for(auto it = begin; it != end; ++it)
  {
    TEETIME_INFO() << "send start signal";
    (*it)->sendSignal(Signal{SignalType::Start});
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
  auto begin = m_stage->getInputPortsBegin();
  auto end = m_stage->getInputPortsEnd();
  for(auto it = begin; it != end; ++it)
  {
    TEETIME_INFO() << "wait fors start signal";
    (*it)->waitForStartSignal();
  }

  TEETIME_INFO() << "Start consumer stage '" << m_stage->getDebugName() << "'";
  m_stage->executeStage();      
}
