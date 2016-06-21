#include "teetime/AbstractOutputPort.h"
#include "teetime/AbstractStage.h"
#include "teetime/Signal.h"

using namespace teetime;

void AbstractOutputPort::sendSignal(const Signal& signal)
{      
  if(auto p = getPipe()) 
  {
    p->addSignal(signal);
  }
}
