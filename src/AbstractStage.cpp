#include <teetime/AbstractStage.h>
#include <exception>
#include <teetime/logging.h>

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
    TEETIME_ERROR() << "stage execution failed: " << e.what();    
  }
  catch( ... )
  {
    TEETIME_ERROR() << "stage execution failed due to unknown error";
  }
}