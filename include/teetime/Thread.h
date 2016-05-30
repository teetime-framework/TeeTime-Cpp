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

  private:
    AbstractStage* m_stage;
    std::thread m_thread;
  };
}