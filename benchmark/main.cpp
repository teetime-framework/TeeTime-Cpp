#include <benchmark/benchmark.h>
#include <teetime/stages/RandomIntProducer.h>
#include <teetime/stages/FunctionStage.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/MergerStage.h>
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/benchmark/ReverseIntMd5Hashing.h>
#include <teetime/ports/Port.h>
#include <teetime/pipes/SynchedPipe.h>
#include <teetime/Configuration.h>
#include <teetime/Md5Hash.h>
#include <climits>
#include <iostream>

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

class SingleThreadedConfig : public Configuration
{
public:
  SingleThreadedConfig(int num, int min, int max) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);    
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();    
    auto sink = createStage<CollectorSink<int>>();

    declareStageActive(producer);

    connectPorts(producer->getOutputPort(), hash->getInputPort());
    connectPorts(hash->getOutputPort(), revhash->getInputPort());
    connectPorts(revhash->getOutputPort(), sink->getInputPort());      
  }
};

class MultihreadedConfig : public Configuration
{
public:
  MultihreadedConfig(int num, int min, int max) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);    
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();
    auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();    
    auto sink = createStage<CollectorSink<int>>();

    declareStageActive(producer);
    declareStageActive(hash);
    declareStageActive(revhash);
    declareStageActive(sink);

    connectPorts(producer->getOutputPort(), hash->getInputPort());
    connectPorts(hash->getOutputPort(), revhash->getInputPort());
    connectPorts(revhash->getOutputPort(), sink->getInputPort());      
  }
};

class DistributedConfig : public Configuration
{
public:
  DistributedConfig(int num, int min, int max, int numThreads) 
  {
    auto producer = createStage<RandomIntProducer>(min, max, num);    
    auto hash = createStageFromFunction<int, Md5Hash, hashInt>();    
    auto sink = createStage<CollectorSink<int>>();
    auto dist = createStage<DistributorStage<Md5Hash>>();
    auto merge = createStage<MergerStage<int>>();

    declareStageActive(producer);        
    declareStageActive(merge);

    for(int i=0; i<numThreads; ++i)
    {
      auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
      declareStageActive(revhash);

      connectPorts(dist->getNewOutputPort(), revhash->getInputPort());      
      connectPorts(revhash->getOutputPort(), merge->getNewInputPort());      
    }


    connectPorts(producer->getOutputPort(), hash->getInputPort());
    connectPorts(hash->getOutputPort(), dist->getInputPort());
    connectPorts(merge->getOutputPort(), sink->getInputPort());          
  }
};


// Define another benchmark
static void TeeTime_SingleThreaded(benchmark::State& state) {
  SingleThreadedConfig config(100000, 100, 1000);
  while (state.KeepRunning())
  { 
    config.executeBlocking();
  }
}
//BENCHMARK(TeeTime_SingleThreaded);

static void TeeTime_MultiThreaded(benchmark::State& state) {
  MultihreadedConfig config(100000, 100, 1000);
  while (state.KeepRunning())
  { 
    config.executeBlocking();
  }
}
//BENCHMARK(TeeTime_MultiThreaded);

static void Optional_Create(benchmark::State& state) {  
  while (state.KeepRunning())
  { 
    Optional<int> o = Optional<int>(42);
    (void)o;
  }
}
BENCHMARK(Optional_Create);

static void Int_Create(benchmark::State& state) {  
  while (state.KeepRunning())
  { 
    int o = 42;
    (void)o;
  }
}
BENCHMARK(Int_Create);

static void HashInt(benchmark::State& state) {  

  int foo = 42;
  while (state.KeepRunning())
  { 
    auto o = Md5Hash::generate(&foo, sizeof(foo));
    (void)o;
  }
}
BENCHMARK(HashInt);


static void HashInt2(benchmark::State& state) {  

  int foo = 42;
  while (state.KeepRunning())
  { 
    auto o1 = Md5Hash::generate(&foo, sizeof(foo));
    auto o2 = Md5Hash::generate(&foo, sizeof(foo));
    (void)o1;
    (void)o2;
  }
}
BENCHMARK(HashInt2);

static void HashInt20(benchmark::State& state) {  

  int foo = 42;
  while (state.KeepRunning())
  { 
    for(int i=0; i<20; ++i)
    {
      auto o1 = Md5Hash::generate(&foo, sizeof(foo));
      (void)o1;
    }
  }
}
BENCHMARK(HashInt20);



BENCHMARK_MAIN()