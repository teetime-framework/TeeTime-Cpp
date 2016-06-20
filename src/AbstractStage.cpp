#include <teetime/AbstractStage.h>
#include <exception>
#include <teetime/logging.h>
#include <teetime/Thread.h>
#include <teetime/Runnable.h>

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
    TEETIME_ERROR() << "stage execution failed: " << e.what();    
  }
  catch( ... )
  {
    TEETIME_ERROR() << "stage execution failed due to unknown error";
  }
}

void AbstractStage::declareActive()
{
  if(!m_runnable)
  {    
    m_runnable = createRunnable();
  }
}

void AbstractStage::declareNonActive()
{
  m_runnable.reset();
}

void AbstractStage::onSignal(const Signal& s)
{
  if(s.type == SignalType::Terminating) 
  {
    setState(StageState::Terminating);
  }

  for (auto p : m_outputPorts)
  {
    p->sendSignal(s);
  }
}

StageState AbstractStage::currentState() const
{
  return m_state;
}

void AbstractStage::setState(StageState state)
{
  m_state = state;
}