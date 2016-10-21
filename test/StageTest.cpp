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
#include <teetime/logging.h>
#include <teetime/stages/AbstractStage.h>
#include <teetime/stages/AbstractConsumerStage.h>
#include <teetime/stages/AbstractProducerStage.h>
#include <teetime/ports/Port.h>
#include <vector>

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
        getOutputPort().send(std::move(i));
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
    virtual void execute(int&& value) override
    {
      TEETIME_INFO() << "processing value: " << value;
      valuesProcessed.push_back(value);
    }
  };
}
#if 0

TEST( AbstractConsumerStageTest, simpleExecute )
{
  TestProducerStage producer;
  TestConsumerStage consumer;   
  
  connect(producer.getOutputPort(), consumer.getInputPort());

  producer.executeStage();

  EXPECT_EQ(size_t(3), consumer.valuesProcessed.size());
  EXPECT_EQ(0, consumer.valuesProcessed[0]);
  EXPECT_EQ(1, consumer.valuesProcessed[1]);
  EXPECT_EQ(2, consumer.valuesProcessed[2]);
}

#endif