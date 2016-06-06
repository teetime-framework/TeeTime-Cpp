#pragma once
#include <thread>
#include "AbstractStage.h"

namespace teetime
{
  class Thread
  {
  public:
    explicit Thread(AbstractStage* stage);

    void start();
    void join();

    static std::thread::id currentId();
    static void currentName(char* buffer, int buffersize);

  private:
    AbstractStage* m_stage;
    std::thread m_thread;
  };
}