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
#include <climits>

using namespace teetime;

template<typename T>
class PrinterStage : public AbstractConsumerStage<T>
{
public:
  explicit PrinterStage(const char* prefix) 
    : m_prefix(prefix)
  {
    m_outputport = AbstractStage::addNewOutputPort<T>();    
  }

  OutputPort<T>& getOutputPort() 
  {
    return *m_outputport;
  }

  virtual void execute(T&& value) override
  {
    std::cout << m_prefix << value << std::endl;
    m_outputport->send(std::move(value));
  }

private:
  std::string    m_prefix;
  OutputPort<T>* m_outputport;
};


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
  Config(int num, int min, int max, bool verbose) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);
    auto logInput = createStage<PrinterStage<int>>("Input:  ");
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
    auto logOutput = createStage<PrinterStage<int>>("Output: ");
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();

    if(verbose) 
    {
      connect(producer->getOutputPort(), logInput->getInputPort());
      connect(logInput->getOutputPort(), hash->getInputPort());
      connect(hash->getOutputPort(), revhash->getInputPort());
      connect(revhash->getOutputPort(), logOutput->getInputPort());      
      connect(logOutput->getOutputPort(), sink->getInputPort());
    }
    else 
    {
      connect(producer->getOutputPort(), hash->getInputPort());
      connect(hash->getOutputPort(), revhash->getInputPort());
      connect(revhash->getOutputPort(), sink->getInputPort());      
    }
  }
};

class Config2 : public Configuration
{
public:
  Config2(int num, int min, int max, int threads, bool verbose) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);
    auto logInput = createStage<PrinterStage<int>>("Input:  ");
    auto distributor = createStage<DistributorStage<int>>();

    auto merger = createStage<MergerStage<int>>();    
    auto logOutput = createStage<PrinterStage<int>>("Output: ");
    auto sink = createStage<CollectorSink<int>>();

    producer->declareActive();
    merger->declareActive();

    for(int i=0; i<threads; ++i)
    {      
      auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
      hash->declareActive();

      auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();

      connect(distributor->getNewOutputPort(), hash->getInputPort());
      connect(hash->getOutputPort(), revhash->getInputPort());
      connect(revhash->getOutputPort(), merger->getNewInputPort());
    }

    if(verbose) 
    {
      connect(producer->getOutputPort(), logInput->getInputPort());
      connect(logInput->getOutputPort(), distributor->getInputPort());
      connect(merger->getOutputPort(), logOutput->getInputPort());      
      connect(logOutput->getOutputPort(), sink->getInputPort());
    }
    else 
    {
      connect(producer->getOutputPort(), distributor->getInputPort());
      connect(merger->getOutputPort(), sink->getInputPort());      
    }
  }
};

int main(int argc, char** argv) 
{
  if(argc < 5) {
    std::cout << "usage: md5 <count> <min> <max> <threads> [verbose]" << std::endl;
    return EXIT_FAILURE;
  }

  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  int num = atoi(argv[1]);
  int min = atoi(argv[2]);
  int max = atoi(argv[3]);
  int threads = atoi(argv[4]);
  bool verbose = (argc > 5 && atoi(argv[5]) > 0);

  std::cout << "settings: num=" << num << ", min=" << min << ", max=" << max << ", threads=" << threads << ", verbose=" << verbose << std::endl;

  if(threads > 0) {
    Config2 config(num, min, max, threads, verbose);
    config.executeBlocking();    
  }
  else {
    Config config(num, min, max, verbose);
    config.executeBlocking();    
  }

  return EXIT_SUCCESS;
}
