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
#include "stages/IntProducerStage.h"
#include "stages/IntConsumerStage.h"

using namespace teetime;
using namespace teetime::test;

namespace
{ 
  class TestConfiguration : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> consumer;

    explicit TestConfiguration()
    {
      producer = createStage<IntProducerStage>();
      consumer = createStage<IntConsumerStage>();

      producer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };

  class TestConfiguration2 : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> consumer;

    TestConfiguration2()
    {
      producer = createStage<IntProducerStage>();
      consumer = createStage<IntConsumerStage>();

      producer->declareActive();
      consumer->declareActive();

      connect(producer->getOutputPort(), consumer->getInputPort());
    }
  };
}

TEST( ConfigurationTest, simple )
{
  TestConfiguration config;
  config.producer->numValues = 5;
  config.producer->startValue = 100;

  config.executeBlocking();

  ASSERT_EQ((size_t)5, config.consumer->valuesConsumed.size());
  EXPECT_EQ(100, config.consumer->valuesConsumed[0]);
  EXPECT_EQ(101, config.consumer->valuesConsumed[1]);
  EXPECT_EQ(102, config.consumer->valuesConsumed[2]);
  EXPECT_EQ(103, config.consumer->valuesConsumed[3]);
  EXPECT_EQ(104, config.consumer->valuesConsumed[4]);
}

TEST(ConfigurationTest, simple2)
{
  TestConfiguration2 config;
  config.producer->numValues = 5;
  config.producer->startValue = 100;  

  config.executeBlocking();

  ASSERT_EQ((size_t)5, config.consumer->valuesConsumed.size());
  EXPECT_EQ(100, config.consumer->valuesConsumed[0]);
  EXPECT_EQ(101, config.consumer->valuesConsumed[1]);
  EXPECT_EQ(102, config.consumer->valuesConsumed[2]);
  EXPECT_EQ(103, config.consumer->valuesConsumed[3]);
  EXPECT_EQ(104, config.consumer->valuesConsumed[4]);
}
