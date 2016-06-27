/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <teetime/Configuration.h>
#include <teetime/stages/AbstractProducerStage.h>
#include <teetime/stages/AbstractConsumerStage.h>

using namespace teetime;

namespace
{  

  template<typename T>
  class DistributorStage : public AbstractConsumerStage<T>
  {
  public:
    DistributorStage()
    : AbstractConsumerStage<T>("DistributorStage")
    , m_next(0)
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

      auto p = unsafe_dynamic_cast<OutputPort<T>>(port);
      
      p->send(value);
      
      m_next = (m_next+1) % AbstractStage::numOutputPorts();
    }    

    uint32 m_next;
  };

  class TestProducerStage : public AbstractProducerStage<int>
  {
  public:
    explicit TestProducerStage()
     : AbstractProducerStage<int>("TestProducerStage")
     , startValue(0)
     , numValues(1)
    {}

    int startValue;
    int numValues;    

  private:
    virtual void execute() override
    {
      for(int i=0; i<numValues; ++i)
      {
        getOutputPort().send(startValue + i);
      }

      terminate();
    }
  };

  class TestConsumerStage : public AbstractConsumerStage<int>
  {
  public:
    std::vector<int> valuesProcessed;

    TestConsumerStage()
    : AbstractConsumerStage<int>("TestConsumerStage")
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

    explicit TestConfiguration()
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
      producer = createStage<TestProducerStage>();
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
    std::vector<shared_ptr<TestConsumerStage>> consumer;


    explicit DistributorTestConfig(unsigned numThreads)
    {
      producer = createStage<TestProducerStage>();
      producer->declareActive();

      auto distributor = createStage<DistributorStage<int>>();
      connect(producer->getOutputPort(), distributor->getInputPort());      

      for(unsigned i=0; i<numThreads; ++i) 
      {
        consumer.push_back(createStage<TestConsumerStage>());
        consumer[i]->declareActive();
        connect(distributor->getNewOutputPort(), consumer[i]->getInputPort());
      }      
    }
  };
}

TEST( ConfigurationTest, simple )
{
  TestConfiguration config;
  config.producer->numValues = 5;
  config.producer->startValue = 100;

  config.executeBlocking();

  ASSERT_EQ((size_t)5, config.consumer->valuesProcessed.size());
  EXPECT_EQ(100, config.consumer->valuesProcessed[0]);
  EXPECT_EQ(101, config.consumer->valuesProcessed[1]);
  EXPECT_EQ(102, config.consumer->valuesProcessed[2]);
  EXPECT_EQ(103, config.consumer->valuesProcessed[3]);
  EXPECT_EQ(104, config.consumer->valuesProcessed[4]);
}

TEST(ConfigurationTest, simple2)
{
  TestConfiguration2 config;
  config.producer->numValues = 5;
  config.producer->startValue = 100;  

  config.executeBlocking();

  ASSERT_EQ((size_t)5, config.consumer->valuesProcessed.size());
  EXPECT_EQ(100, config.consumer->valuesProcessed[0]);
  EXPECT_EQ(101, config.consumer->valuesProcessed[1]);
  EXPECT_EQ(102, config.consumer->valuesProcessed[2]);
  EXPECT_EQ(103, config.consumer->valuesProcessed[3]);
  EXPECT_EQ(104, config.consumer->valuesProcessed[4]);
}

TEST(ConfigurationTest, distributor)
{
  DistributorTestConfig config(4);
  config.producer->numValues = 16;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)4, config.consumer.size());
  EXPECT_EQ((size_t)4, config.consumer[0]->valuesProcessed.size());
  EXPECT_EQ((size_t)4, config.consumer[1]->valuesProcessed.size());
  EXPECT_EQ((size_t)4, config.consumer[2]->valuesProcessed.size());
  EXPECT_EQ((size_t)4, config.consumer[3]->valuesProcessed.size());
}