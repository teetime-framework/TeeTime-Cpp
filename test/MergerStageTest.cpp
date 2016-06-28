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
#include <teetime/stages/MergerStage.h>
#include "stages/IntProducerStage.h"
#include "stages/IntConsumerStage.h"

using namespace teetime;
using namespace teetime::test;

namespace 
{
  class MergerTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> consumer;

    explicit MergerTestConfig(unsigned numOutputPorts)
    {
      //create stages
      producer = createStage<IntProducerStage>();      
      auto distributor = createStage<DistributorStage<int>>();
      auto merger = createStage<MergerStage<int>>();      
      consumer = createStage<IntConsumerStage>();

      //make stages active
      producer->declareActive();

      //connect stages
      connect(producer->getOutputPort(), distributor->getInputPort()); 

      for(unsigned i=0; i<numOutputPorts; ++i) 
      {
        connect(distributor->getNewOutputPort(), merger->getNewInputPort());      
      }         

      connect(merger->getOutputPort(), consumer->getInputPort());      
    }
  };
}

TEST(MergerStageTest, singleInput)
{
  MergerTestConfig config(1);
  config.producer->numValues = 10;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)10, config.consumer->valuesConsumed.size());
}

TEST(MergerStageTest, multipleInput)
{
  MergerTestConfig config(10);
  config.producer->numValues = 10;
  config.producer->startValue = 0;   

  config.executeBlocking();

  ASSERT_EQ((size_t)10, config.consumer->valuesConsumed.size());
}