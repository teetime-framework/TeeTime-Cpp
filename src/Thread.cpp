#include <teetime/Thread.h>
#include <teetime/logging.h>
using namespace teetime;

Thread::Thread(AbstractStage* stage)
  : m_stage(stage)
{
  
}

void Thread::start()
{
  if(!m_stage) {
    TEETIME_ERROR() << "No stage to execute";
    return;
  }

  m_thread = std::thread([&](){
    m_stage->executeStage();
  });
}

void Thread::join()
{
  m_thread.join();
}

std::thread::id Thread::currentId()
{
  return std::this_thread::get_id();
}

void Thread::currentName(char * buffer, int buffersize)
{
  if (buffer && buffersize > 0)
  {
    *buffer = '\0';
  }
}
