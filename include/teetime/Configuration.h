#pragma once
#include "Thread.h"
#include "AbstractStage.h"

namespace teetime
{
  class Configuration
  {
  public:
    Configuration()
    {
    }

    ~Configuration()
    {
    }

    void executeBlocking()
    {
      executeNonBlocking();
      waitForFinished();
    }

    void executeNonBlocking()
    {
      for(const auto& s : m_stages)
      {
        if(auto thread = s->getOwningThread())
        {
          thread->start();
        }
      }
    }

    void waitForFinished()
    {
      for(const auto& s : m_stages)
      {
        if(auto thread = s->getOwningThread())
        {
          thread->join();
        }
      }      
    }

    template<typename T, typename ...TArgs>
    shared_ptr<T> createStage(TArgs... args)
    {
      auto stage = std::make_shared<T>(args...);
      m_stages.push_back(stage);
      return stage;
    }

  private:    
    std::vector<shared_ptr<AbstractStage>> m_stages;
  };
}