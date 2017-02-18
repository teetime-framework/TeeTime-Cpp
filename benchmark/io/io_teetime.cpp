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
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/MergerStage.h> 
#include <teetime/Configuration.h>
#include <teetime/Md5Hash.h>
#include <teetime/logging.h>
#include <teetime/platform.h>
#include <climits>
#include <string>
#include <random>
#include <fstream>
#include "../Benchmark.h"

using namespace teetime;


int writeAndReadFile(const char* fileprefix, int fileNum, const std::vector<char>& writeBuffer, std::vector<char>& readBuffer, int size);

namespace {

  class Producer : public AbstractProducerStage<int>
  {
  public:
    Producer(int min, int max, int num)
      : m_min(min)
      , m_max(max)
      , m_num(num)
    {}

  private:
    virtual void execute() override
    {
      //std::random_device                  rand_dev;
      std::mt19937                        generator(0); //TODO(johl): currently using 0 as seed (instead of rand_dev) for reproducable results. This should be adjustable.
      std::uniform_int_distribution<int>  distr(m_min, m_max);

      for (int i = 0; i < m_num; ++i)
      {
        int value = distr(generator);
        AbstractProducerStage<int>::getOutputPort().send(std::move(value));
      }

      AbstractProducerStage<int>::terminate();
    }

    int m_min;
    int m_max;
    int m_num;
  };

  class WriterReader : public AbstractFilterStage<int, int>
  {
  public:
    explicit WriterReader(const char* filePrefix)
      : m_filePrefix(filePrefix)
      , m_counter(0)
    {
      m_writeBuffer.resize(1024 * 1024 * 100, 'X');
      m_readBuffer.resize(1024 * 1024 * 100, '\0');
    }
  
  private:
    virtual void execute(int&& value) override
    {
      assert(value < m_writeBuffer.size());
      assert(value < m_readBuffer.size());

      int ret = writeAndReadFile(m_filePrefix.c_str(), m_counter++, m_writeBuffer, m_readBuffer, value);

      getOutputPort().send(std::move(ret));
    }

    std::string              m_filePrefix;
    int                      m_counter;
    std::vector<char>        m_writeBuffer;
    std::vector<char>        m_readBuffer;
  };

  class Config : public Configuration
  {
  public:
    Config(int num, int min, int max, int threads, const std::vector<int>& affinity)
    {
      CpuDispenser cpus(affinity);

      auto producer = createStage<Producer>(min, max, num);
      auto dist = createStage<DistributorStage<int>>();
      auto merger = createStage<MergerStage<int>>();
      auto sink = createStage<CollectorSink<int>>();

      declareStageActive(producer, cpus.next());
      declareStageActive(merger, cpus.next());

      for (int i = 0; i < threads; ++i)
      {
        char prefix[256];
        sprintf(prefix, "writer%d_", i);
        auto writerReader = createStage<WriterReader>(prefix);        

        declareStageActive(writerReader, cpus.next());

        connectPorts(dist->getNewOutputPort(), writerReader->getInputPort());
        connectPorts(writerReader->getOutputPort(), merger->getNewInputPort());
      }

      connectPorts(producer->getOutputPort(), dist->getInputPort());
      connectPorts(merger->getOutputPort(), sink->getInputPort());
    }
  };
}

void io_teetime_noAffinity(const Params& params, int threads)
{
  Config config(params.getInt32("num"), params.getInt32("minvalue"), params.getInt32("maxvalue"), threads, affinity_none);
  config.executeBlocking();
}

void io_teetime_preferSameCpu(const Params& params, int threads)
{
  Config config(params.getInt32("num"), params.getInt32("minvalue"), params.getInt32("maxvalue"), threads, affinity_preferSameCpu);
  config.executeBlocking();
}

void io_teetime_avoidSameCore(const Params& params, int threads)
{
  Config config(params.getInt32("num"), params.getInt32("minvalue"), params.getInt32("maxvalue"), threads, affinity_avoidSameCore);
  config.executeBlocking();
}