#include <teetime/AbstractStage.h>
#include <exception>
#include <teetime/logging.h>
#include <teetime/Thread.h>

using namespace teetime;

AbstractStage::AbstractStage()
{

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
  if(!m_thread)
  {
    m_thread.reset(new Thread(this));
  }
}

void AbstractStage::declareNonActive()
{
  m_thread.reset();
}