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
#include <teetime/stages/InitialElementProducer.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/FunctionStage.h>
#include <teetime/ports/Port.h>
#include <teetime/File.h>
#include <teetime/FileBuffer.h>

using namespace teetime;

namespace
{
  double divideBy2(int i)
  {
    return i * 0.5;
  }

  class FunctionPtrStageTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<double>> collector;

    explicit FunctionPtrStageTestConfig()
    {
      auto producer = createStage<InitialElementProducer<int>>(std::vector<int>{1,2,3});
      auto function = createStage<FunctionPtrStage<int,double>>(divideBy2, "divideBy2");
      collector = createStage<CollectorSink<double>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), function->getInputPort());
      connectPorts(function->getOutputPort(), collector->getInputPort());
    }
  };

  class FunctionPtrStageTest2Config : public Configuration
  {
  public:
    shared_ptr<CollectorSink<double>> collector;

    explicit FunctionPtrStageTest2Config()
    {
      auto producer = createStage<InitialElementProducer<int>>(std::vector<int>{1, 2, 3});
      auto function = createStage<NewFunctionStage<int, double>>(divideBy2);
      collector = createStage<CollectorSink<double>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), function->getInputPort());
      connectPorts(function->getOutputPort(), collector->getInputPort());
    }
  };
  
  class FunctionObjectStageTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<double>> collector;

    explicit FunctionObjectStageTestConfig()
    {
      auto producer = createStage<InitialElementProducer<int>>(std::vector<int>{1, 2, 3});

      auto function = createStageFromFunctionObject<int, double>([](int i) { return i + 1; });

      collector = createStage<CollectorSink<double>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), function->getInputPort());
      connectPorts(function->getOutputPort(), collector->getInputPort());
    }
  };

}

TEST(FunctionPtrStageTest, simple)
{
  FunctionPtrStageTestConfig config;

  config.executeBlocking();

  std::vector<double> d = config.collector->takeElements();

  ASSERT_EQ((size_t)3, d.size());
  EXPECT_DOUBLE_EQ(0.5, d[0]);
  EXPECT_DOUBLE_EQ(1.0, d[1]);
  EXPECT_DOUBLE_EQ(1.5, d[2]);
}

TEST(FunctionPtrStageTest, createdFromConfig)
{
  FunctionPtrStageTest2Config config;

  config.executeBlocking();

  std::vector<double> d = config.collector->takeElements();

  ASSERT_EQ((size_t)3, d.size());
  EXPECT_DOUBLE_EQ(0.5, d[0]);
  EXPECT_DOUBLE_EQ(1.0, d[1]);
  EXPECT_DOUBLE_EQ(1.5, d[2]);
}


TEST(FunctionObjectTest, createdFromConfig)
{
  FunctionObjectStageTestConfig config;

  config.executeBlocking();

  std::vector<double> d = config.collector->takeElements();

  ASSERT_EQ((size_t)3, d.size());
  EXPECT_DOUBLE_EQ(2.0, d[0]);
  EXPECT_DOUBLE_EQ(3.0, d[1]);
  EXPECT_DOUBLE_EQ(4.0, d[2]);
}