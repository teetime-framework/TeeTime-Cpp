#include <gtest/gtest.h>
#include <teetime/Configuration.h>
#include <teetime/AbstractProducerStage.h>
#include <teetime/AbstractConsumerStage.h>

using namespace teetime;

namespace
{  
  template<typename T>
  class DistributorStage : public AbstractConsumerStage<T>
  {
  public:
    DistributorStage()
    : m_next(0)
    {}

    OutputPort<T>& getNewOutputPort() 
    {
      OutputPort<T>* p = AbstractStage::addNewOutputPort<T>();

      return *p;
    }

  private:
    virtual void execute(const int& value)
    {
      TEETIME_INFO() << "distributing value '" << value << "' to port " << m_next;
      auto port = AbstractStage::getOutputPort(m_next);

      assert(dynamic_cast<OutputPort<T>*>(port) != nullptr);
      auto p = reinterpret_cast<OutputPort<T>*>(port);
      
      p->send(value);
      
      m_next = (m_next+1) % AbstractStage::numOutputPorts();
    }    

    uint32 m_next;
  };

  class TestProducerStage : public AbstractProducerStage<int>
  {
  public:
    explicit TestProducerStage(int startValue = 0)
     : startValue(startValue)
    {}

  private:
    virtual void execute() override
    {
      for(int i=0; i<3; ++i)
      {
        getOutputPort().send(startValue + i);
      }
    }

    int startValue;
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

  class TestConfiguration2 : public Configuration
  {
  public:
    TestConfiguration2()
    {
      auto producer = createStage<TestProducerStage>(42);
      auto consumer = createStage<TestConsumerStage>();

      producer->declareActive();
      consumer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };


  class DistributorTestConfig : public Configuration
  {
  public:
    DistributorTestConfig()
    {
      auto producer = createStage<TestProducerStage>();
      auto distributor = createStage<DistributorStage<int>>();
      auto consumer1 = createStage<TestConsumerStage>();
      auto consumer2 = createStage<TestConsumerStage>();
      auto consumer3 = createStage<TestConsumerStage>();
      auto consumer4 = createStage<TestConsumerStage>();

      producer->declareActive();
      consumer1->declareActive();
      consumer2->declareActive();
      consumer3->declareActive();
      consumer4->declareActive();

      connect(producer->getOutputPort(), distributor->getInputPort());
      connect(distributor->getNewOutputPort(), consumer1->getInputPort());
      connect(distributor->getNewOutputPort(), consumer2->getInputPort());
      connect(distributor->getNewOutputPort(), consumer3->getInputPort());
      connect(distributor->getNewOutputPort(), consumer4->getInputPort());
    }
  };

}

TEST( ConfigurationTest, simple )
{
  TestConfiguration config;

  config.executeBlocking();
}

TEST(ConfigurationTest, simple2)
{
  TestConfiguration2 config;

  config.executeBlocking();
}

TEST(ConfigurationTest, distributor)
{
  DistributorTestConfig config;

  config.executeBlocking();
}