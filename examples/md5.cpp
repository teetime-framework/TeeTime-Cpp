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

using namespace teetime;

Md5Hash hashInt(int i) {
  return Md5Hash::generate(&i, sizeof(i));
}

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
    auto producer = createStage<RandomIntProducer>(min, max, num);
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();

    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());      
  }
};

class Config2 : public Configuration
{
public:
  Config2(int num, int min, int max, int threads) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);
    auto distributor = createStage<DistributorStage<Md5Hash>>();
    auto merger = createStage<MergerStage<int>>();
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
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

    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), distributor->getInputPort());
    connect(merger->getOutputPort(), sink->getInputPort());      
  }
};

class Config3 : public Configuration
{
public:
  Config3(int num, int min, int max) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();
    hash->declareActive();
    revhash->declareActive();
    sink->declareActive();

    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());
  }
};

int main(int argc, char** argv) 
{
  if(argc < 5) {
    std::cout << "usage: md5 <count> <min> <max> <threads>" << std::endl;
    return EXIT_FAILURE;
  }

  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  int num = atoi(argv[1]);
  int min = atoi(argv[2]);
  int max = atoi(argv[3]);
  int threads = atoi(argv[4]);

  std::cout << "settings: num=" << num << ", min=" << min << ", max=" << max << ", threads=" << threads << std::endl;

  auto start = platform::microSeconds();

  if(threads > 0) {
    Config2 config(num, min, max, threads);
    config.executeBlocking();    
  }
  else if(threads == 0) {
    Config config(num, min, max);
    config.executeBlocking();    
  } 
  else {
    Config3 config(num, min, max);
    config.executeBlocking();        
  }

  std::cout << "measured time: " << (platform::microSeconds() - start) * 0.001 << "ms" << std::endl;

  return EXIT_SUCCESS;
}
