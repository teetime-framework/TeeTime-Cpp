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
#include <teetime/stages/DelayStage.h>
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/MergerStage.h>
#include "stages/IntProducerStage.h"
#include "stages/IntConsumerStage.h"

using namespace teetime;
using namespace teetime::test;

namespace
{
  class SimpleDelayStageTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> consumer;

    explicit SimpleDelayStageTestConfig(uint32 milliseconds)
    {
      producer = createStage<IntProducerStage>();
      auto delay = createStage<DelayStage<int>>(milliseconds);
      consumer = createStage<IntConsumerStage>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), delay->getInputPort());
      connectPorts(delay->getOutputPort(), consumer->getInputPort());
    }
  };
}

TEST(DelayStageTest, simple)
{
  SimpleDelayStageTestConfig config(1000);
  config.producer->numValues = 3;
  config.producer->startValue = 0;

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  config.executeBlocking();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  TEETIME_TRACE() << "time: " << time;
  EXPECT_TRUE(time >= 3000);
}

namespace
{
  class ParallelDelayStageTestConfig : public Configuration
  {
  public:
    shared_ptr<IntProducerStage> producer;
    shared_ptr<IntConsumerStage> consumer;

    explicit ParallelDelayStageTestConfig(uint32 milliseconds)
    {
      producer = createStage<IntProducerStage>();
      auto distributor = createStage<DistributorStage<int>>();
      declareStageActive(producer);

      auto merger = createStage<MergerStage<int>>();
      consumer = createStage<IntConsumerStage>();
      declareStageActive(merger);

      connectPorts(producer->getOutputPort(), distributor->getInputPort());
      for(int i=0; i<3; ++i)
      {
        std::string delayStageName = "DelayStage" + std::to_string(i);

        auto delay = createStage<DelayStage<int>>(milliseconds, delayStageName.c_str());
        declareStageActive(delay);

        connectPorts(distributor->getNewOutputPort(), delay->getInputPort());
        connectPorts(delay->getOutputPort(), merger->getNewInputPort());
      }

      connectPorts(merger->getOutputPort(), consumer->getInputPort());
    }
  };
}


TEST(DelayStageTest, parallel)
{
  ParallelDelayStageTestConfig config(1000);
  config.producer->numValues = 3;
  config.producer->startValue = 0;

  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
  config.executeBlocking();
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  TEETIME_INFO() << "time: " << time;
  const decltype(time) tolerance = 200;
  EXPECT_TRUE(time >= 1000);
  EXPECT_TRUE(time < (1000 + tolerance));
}

