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
      producer->declareActive();

      auto distributor = createStage<DistributorStage<int>>();
      connect(producer->getOutputPort(), distributor->getInputPort());      

      for(unsigned i=0; i<numOutputPorts; ++i) 
      {
        consumer.push_back(createStage<IntConsumerStage>());

        if(multithreaded)
        {
          consumer[i]->declareActive();
        }
        
        connect(distributor->getNewOutputPort(), consumer[i]->getInputPort());
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