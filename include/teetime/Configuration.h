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
      std::vector<std::thread> threads;

      for(const auto& s : m_stages)
      {
        if(auto runnable = s->getRunnable())
        {
          std::thread t([=](){
            runnable->run();
          });

          threads.push_back(std::move(t));
        }
      }      

      for(auto& t : threads)
      {
        t.join();
      }      
    }
    

#if 0
    void executeNonBlocking()
    {
      for(const auto& s : m_stages)
      {
        if(auto thread = s->getExecutingThread())
        {
          thread->start();
        }
      }
    }

    void waitForFinished()
    {
      for(const auto& s : m_stages)
      {
        if(auto thread = s->getExecutingThread())
        {
          thread->join();
        }
      }      
    }
#endif
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