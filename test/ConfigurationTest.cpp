#include <gtest/gtest.h>
#include <teetime/Configuration.h>
#include <teetime/AbstractProducerStage.h>
#include <teetime/AbstractConsumerStage.h>

using namespace teetime;

namespace
{  
  class TestProducerStage : public AbstractProducerStage<int>
  {
  public:

  private:
    virtual void execute() override
    {
      for(int i=0; i<3; ++i)
      {
        getOutputPort().send(i);
      }
    }
  };

  class TestConsumerStage : public AbstractConsumerStage<int>
  {
  public:
    std::vector<int> valuesProcessed;

    TestConsumerStage()
    {      
    }

  private:
    virtual void execute(const int& value)
    {
      TEETIME_INFO() << "processing value: " << value;
      valuesProcessed.push_back(value);
    }
  };

  class TestConfiguration : public Configuration
  {
  public:
    TestConfiguration()
    {
      auto producer = createStage<TestProducerStage>();    
      auto consumer = createStage<TestConsumerStage>();

      producer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };
}

TEST( ConfigurationTest, simple )
{
  TestConfiguration config;

  config.executeBlocking();
}
