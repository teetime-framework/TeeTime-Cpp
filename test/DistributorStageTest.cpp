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
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/DelayStage.h> 
#include "stages/IntProducerStage.h"
#include "stages/IntConsumerStage.h"


using namespace teetime;
using namespace teetime::test;

namespace 
{
  class DistributorTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    std::vector<shared_ptr<IntConsumerStage>> consumer;

    explicit DistributorTestConfig(unsigned numOutputPorts, bool multithreaded)
    {
      producer = createStage<IntProducerStage>();
      declareStageActive(producer);

      auto distributor = createStage<DistributorStage<int, BlockingRoundRobinDistribution<int>>>();
      connectPorts(producer->getOutputPort(), distributor->getInputPort());      

      for(unsigned i=0; i<numOutputPorts; ++i) 
      {
        consumer.push_back(createStage<IntConsumerStage>());

        if(multithreaded)
        {
          declareStageActive(consumer[i]);
        }
        
        connectPorts(distributor->getNewOutputPort(), consumer[i]->getInputPort());
      }      
    }
  };
}

TEST(DistributorStageTest, singlethreaded)
{
  DistributorTestConfig config(4, false);
  config.producer->numValues = 16;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)4, config.consumer.size());
  EXPECT_EQ((size_t)4, config.consumer[0]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[1]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[2]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[3]->valuesConsumed.size());
}

TEST(DistributorStageTest, multithreaded)
{
  DistributorTestConfig config(4, true);
  config.producer->numValues = 16;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)4, config.consumer.size());
  EXPECT_EQ((size_t)4, config.consumer[0]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[1]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[2]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[3]->valuesConsumed.size());
}

TEST(DistributorStageTest, nooutput)
{
  DistributorTestConfig config(0, false);
  config.producer->numValues = 16;
  config.producer->startValue = 0;

  EXPECT_EQ((size_t)0, config.consumer.size());
}









namespace 
{



  class RoundRobinTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> delayedConsumer;
    shared_ptr<IntConsumerStage> nonDelayedConsumer1;
    shared_ptr<IntConsumerStage> nonDelayedConsumer2;

    explicit RoundRobinTestConfig()
    {
      producer = createStage<IntProducerStage>();
      declareStageActive(producer);

      auto distributor = createStage<DistributorStage<int, RoundRobinDistribution<int>>>();
      connectPorts(producer->getOutputPort(), distributor->getInputPort());      

      auto delay = createStage<DelayStage<int>>(500);

      delayedConsumer = createStage<IntConsumerStage>();
      nonDelayedConsumer1 = createStage<IntConsumerStage>();
      nonDelayedConsumer2 = createStage<IntConsumerStage>();

      declareStageActive(delay);
      declareStageActive(nonDelayedConsumer1);
      declareStageActive(nonDelayedConsumer2);

      connectPorts(delay->getOutputPort(), delayedConsumer->getInputPort());

      connectPorts(distributor->getNewOutputPort(), delay->getInputPort(), 2);
      connectPorts(distributor->getNewOutputPort(), nonDelayedConsumer1->getInputPort(), 1024);
      connectPorts(distributor->getNewOutputPort(), nonDelayedConsumer2->getInputPort(), 1024);
    }
  };
}

TEST(RoundRobinTest, simple)
{
  RoundRobinTestConfig config;
  config.producer->numValues = 16;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)2, config.delayedConsumer->valuesConsumed.size());
  ASSERT_EQ((size_t)7, config.nonDelayedConsumer1->valuesConsumed.size());
  ASSERT_EQ((size_t)7, config.nonDelayedConsumer2->valuesConsumed.size());

  EXPECT_EQ(0, config.delayedConsumer->valuesConsumed[0]);
  EXPECT_EQ(1, config.nonDelayedConsumer1->valuesConsumed[0]);
  EXPECT_EQ(2, config.nonDelayedConsumer2->valuesConsumed[0]);

  EXPECT_EQ(3, config.delayedConsumer->valuesConsumed[1]);  
  EXPECT_EQ(4, config.nonDelayedConsumer1->valuesConsumed[1]);
  EXPECT_EQ(5, config.nonDelayedConsumer2->valuesConsumed[1]);

  EXPECT_EQ(6, config.nonDelayedConsumer1->valuesConsumed[2]);
  EXPECT_EQ(7, config.nonDelayedConsumer2->valuesConsumed[2]);

  EXPECT_EQ(8, config.nonDelayedConsumer1->valuesConsumed[3]);
  EXPECT_EQ(9, config.nonDelayedConsumer2->valuesConsumed[3]);

  EXPECT_EQ(10, config.nonDelayedConsumer1->valuesConsumed[4]);
  EXPECT_EQ(11, config.nonDelayedConsumer2->valuesConsumed[4]);

  EXPECT_EQ(12, config.nonDelayedConsumer1->valuesConsumed[5]);
  EXPECT_EQ(13, config.nonDelayedConsumer2->valuesConsumed[5]);

  EXPECT_EQ(14, config.nonDelayedConsumer1->valuesConsumed[6]);
  EXPECT_EQ(15, config.nonDelayedConsumer2->valuesConsumed[6]);
  
}





namespace
{
  class CopyTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    std::vector<shared_ptr<IntConsumerStage>> consumer;

    explicit CopyTestConfig(unsigned numOutputPorts, bool multithreaded)
    {
      producer = createStage<IntProducerStage>();
      declareStageActive(producer);

      auto distributor = createStage<DistributorStage<int, CopyDistribution<int>>>();
      connectPorts(producer->getOutputPort(), distributor->getInputPort());

      for (unsigned i = 0; i < numOutputPorts; ++i)
      {
        consumer.push_back(createStage<IntConsumerStage>());

        if (multithreaded)
        {
          declareStageActive(consumer[i]);
        }

        connectPorts(distributor->getNewOutputPort(), consumer[i]->getInputPort());
      }
    }
  };
}

TEST(DistributorCopyTest, singlethreaded)
{
  CopyTestConfig config(4, false);
  config.producer->numValues = 4;
  config.producer->startValue = 0;

  config.executeBlocking();

  ASSERT_EQ((size_t)4, config.consumer.size());
  EXPECT_EQ((size_t)4, config.consumer[0]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[1]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[2]->valuesConsumed.size());
  EXPECT_EQ((size_t)4, config.consumer[3]->valuesConsumed.size());
}






