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

using namespace teetime;

namespace {

class Producer : public AbstractProducerStage<Md5Hash>
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
    if (m_min == m_max)
    {
      auto hash = Md5Hash::generate(&m_min, sizeof(m_min));
      for (unsigned i = 0; i < m_num; ++i)
      {
        AbstractProducerStage<Md5Hash>::getOutputPort().send(Md5Hash(hash));
      }
    }
    else
    {
      //std::random_device                  rand_dev;
      std::mt19937                        generator(0); //TODO(johl): currently using 0 as seed (instead of rand_dev) for reproducable results. This should be adjustable.
      std::uniform_int_distribution<int>  distr(m_min, m_max);

      for (unsigned i = 0; i < m_num; ++i)
      {
        int value = distr(generator);
        AbstractProducerStage<Md5Hash>::getOutputPort().send(Md5Hash::generate(&value, sizeof(value)));
      }
    }


    AbstractProducerStage<Md5Hash>::terminate();
  }

  int m_min;
  int m_max;
  int m_num;
};

int reverseHash(Md5Hash hash) {
  for(int i=0; i<INT_MAX; ++i) {
    if( Md5Hash::generate(&i, sizeof(i)) == hash ) {
      return i;
    }
  }

  return -1;
}

class Config : public Configuration
{
public:
  Config(int num, int min, int max) 
  {
    auto producer = createStage<Producer>(min, max, num);
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();

    connect(producer->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());      
  }
};

static int cpus[] = {
  0,1,2,3,4,5,6,7,16,17,18,19,20,21,22,23
};

static int cpus2[] = {
  0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
};

class Config2 : public Configuration
{
public:
  Config2(int num, int min, int max, int threads) 
  {
    auto producer = createStage<Producer>(min, max, num);
    auto distributor = createStage<DistributorStage<Md5Hash>>();
    auto merger = createStage<MergerStage<int>>();
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();
    merger->declareActive();

    for(int i=0; i<threads; ++i)
    {
      auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
      revhash->declareActive();

      connect(distributor->getNewOutputPort(), revhash->getInputPort());
      connect(revhash->getOutputPort(), merger->getNewInputPort());
    }

    connect(producer->getOutputPort(), distributor->getInputPort());
    connect(merger->getOutputPort(), sink->getInputPort());      
  }
};

class Config3 : public Configuration
{
public:
  Config3(int num, int min, int max) 
  {
    auto producer = createStage<Producer>(min, max, num);
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();
    revhash->declareActive();
    sink->declareActive();

    connect(producer->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());
  }
};

}

void benchmark_teetime(int num, int min, int max, int threads)
{
  if (threads > 0) {
    Config2 config(num, min, max, threads);
    config.executeBlocking();
  }
  else if (threads == 0) {
    Config config(num, min, max);
    config.executeBlocking();
  }
  else {
    Config3 config(num, min, max);
    config.executeBlocking();
  }
}