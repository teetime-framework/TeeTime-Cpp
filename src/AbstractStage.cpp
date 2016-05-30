#include <teetime/AbstractStage.h>
#include <exception>

using namespace teetime;

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
    
  }
  catch( ... )
  {

  }
}