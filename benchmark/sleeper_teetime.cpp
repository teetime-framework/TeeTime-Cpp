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

#include <iostream>
#include <teetime/stages/RandomIntProducer.h>
#include <teetime/stages/AbstractConsumerStage.h>
#include <teetime/stages/AbstractFilterStage.h>
#include <teetime/stages/FunctionStage.h>
#include <teetime/stages/DelayStage.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/MergerStage.h> 
#include <teetime/Configuration.h>
#include <teetime/Md5Hash.h>
#include <teetime/ports/Port.h>
#include <teetime/logging.h>
#include <teetime/platform.h>
#include <climits>
#include <string>
#include <random>
#include <fstream>

using namespace teetime;



namespace {
  uint64 start;

  class Foo : public AbstractFilterStage<int, int>
  {
  public:
    explicit Foo(const char* debugName)
      : AbstractFilterStage(debugName)
      , num(0)
    {}

  private:
    virtual void execute(int&& i) override
    {
      ++num;
      TEETIME_INFO() << this->debugName() << "finished execution " << num << " after " << (platform::microSeconds() - start) * 0.001 << "ms";
      getOutputPort().send(std::move(i));
    }

    int num;
  };

  class Config : public Configuration
  {
  public:
    Config(int num, int min, int max, int threads)
    {
      auto producer = createStage<RandomIntProducer>(min, max, num);
      auto dist = createStage<DistributorStage<int>>();
      auto merger = createStage<MergerStage<int>>();
      auto sink = createStage<CollectorSink<int>>();

      producer->declareActive(0);
      merger->declareActive(0);

      connect(producer->getOutputPort(), dist->getInputPort());
      for (int i = 0; i < threads; ++i)
      {
        char name[128];
        sprintf(name, "delaystage%d", i);

        auto delay = createStage<DelayStage<int>>(10000, name);
        //auto logger = createStage<Foo>(name);
        delay->declareActive(0);

        connect(dist->getNewOutputPort(), delay->getInputPort());
        connect(delay->getOutputPort(), merger->getNewInputPort());
        //connect(delay->getOutputPort(), logger->getInputPort());
        //connect(logger->getOutputPort(), merger->getNewInputPort());
      }
/*
      auto mergerLogger = createStage<Foo>("merger");
      connect(merger->getOutputPort(), mergerLogger->getInputPort());
      */
      connect(merger->getOutputPort(), sink->getInputPort());
    }
  };

}

void sleeper_benchmark_teetime(int num, int min, int max, int threads)
{
  Config config(num, min, max, threads);
  start = platform::microSeconds();
  config.executeBlocking();
}