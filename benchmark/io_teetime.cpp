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
#include <teetime/stages/FunctionStage.h>
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

  class Writer : public AbstractConsumerStage<int>
  {
  public:
    explicit Writer(const char* filePrefix)
      : m_filePrefix(filePrefix)
      , m_counter(0)
    {
      m_outputPort = AbstractConsumerStage<int>::addNewOutputPort<std::string>();
    }

    OutputPort<std::string>& getOutputPort()
    {
      return *m_outputPort;
    }

  private:
    virtual void execute(int&& value) override
    {
      char filename[256];
      sprintf(filename, "%s%d", m_filePrefix.c_str(), m_counter++);

      std::ofstream file;
      file.open(filename, std::ios_base::out | std::ios_base::binary);

      uint8 b = static_cast<uint8>(value % 256);
      for (int i = 0; i < value; ++i)
      {
        file.write(reinterpret_cast<const char*>(&b), 1);
      }
      file.close();

      m_outputPort->send(std::string(filename));
    }

    OutputPort<std::string>* m_outputPort;
    std::string              m_filePrefix;
    int                      m_counter;
  };

  class Reader : public AbstractConsumerStage<std::string>
  {
  public:
    explicit Reader()
    {
      m_outputPort = AbstractConsumerStage<std::string>::addNewOutputPort<int>();
    }

    OutputPort<int>& getOutputPort()
    {
      return *m_outputPort;
    }

  private:
    virtual void execute(std::string&& value) override
    {
      std::ifstream file;
      file.open(value, std::ios_base::in | std::ios_base::binary);

      int count = 0;
      while (true)
      {
        uint8 b;
        file.read(reinterpret_cast<char*>(&b), 1);

        if (file.eof())
          break;
        else
          count += 1;
      }
      
      m_outputPort->send(std::move(count));
    }

    OutputPort<int>* m_outputPort;
  };

  class Config : public Configuration
  {
  public:
    Config(int num, int min, int max, int threads)
    {
      unused(threads);

      auto producer = createStage<Producer>(min, max, num);
      auto writer = createStage<Writer>("writer0_");
      auto reader = createStage<Reader>();
      auto sink = createStage<CollectorSink<int>>();

      producer->declareActive();     
      reader->declareActive();

      connect(producer->getOutputPort(), writer->getInputPort());
      connect(writer->getOutputPort(), reader->getInputPort());
      connect(reader->getOutputPort(), sink->getInputPort());
    }
  };

}

void io_benchmark_teetime(int num, int min, int max, int threads)
{  
  Config config(num, min, max, threads);
  config.executeBlocking(); 
}