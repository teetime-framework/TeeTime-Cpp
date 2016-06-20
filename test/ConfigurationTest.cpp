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
      for(int i=0; i<10; ++i)
      {
        getOutputPort().send(startValue + i);
      }

      getOutputPort().sendSignal(Signal{SignalType::Terminating});
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
    shared_ptr<TestProducerStage> producer;
    shared_ptr<TestConsumerStage> consumer;

    TestConfiguration()
    {
      producer = createStage<TestProducerStage>();    
      consumer = createStage<TestConsumerStage>();

      producer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };

  class TestConfiguration2 : public Configuration
  {
  public:
    shared_ptr<TestProducerStage> producer;
    shared_ptr<TestConsumerStage> consumer;

    TestConfiguration2()
    {
      producer = createStage<TestProducerStage>(42);
      consumer = createStage<TestConsumerStage>();

      producer->declareActive();
      consumer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };


  class DistributorTestConfig : public Configuration
  {
  public:
    shared_ptr<TestProducerStage> producer;
    shared_ptr<TestConsumerStage> consumer[4];


    DistributorTestConfig()
    {
      producer = createStage<TestProducerStage>();
      producer->declareActive();

      auto distributor = createStage<DistributorStage<int>>();
      connect(producer->getOutputPort(), distributor->getInputPort());      

      for(int i=0; i<4; ++i) 
      {
        consumer[i] = createStage<TestConsumerStage>();
        consumer[i]->declareActive();
        connect(distributor->getNewOutputPort(), consumer[i]->getInputPort());
      }      
    }

    bool validate() {
      int num = 0;
      for(int i=0; i<4; ++i) 
      {
        num += consumer[i]->valuesProcessed.size();
      }

      return num == 10;
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

  EXPECT_EQ(10, config.consumer->valuesProcessed.size());
}

TEST(ConfigurationTest, distributor)
{
  DistributorTestConfig config;

  config.executeBlocking();

  //EXPECT_TRUE(config.validate());
}