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

    producer->declareActive();

    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());      
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

    producer->declareActive();
    hash->declareActive();
    revhash->declareActive();
    sink->declareActive();

    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), revhash->getInputPort());
    connect(revhash->getOutputPort(), sink->getInputPort());      
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

    producer->declareActive();        
    merge->declareActive();

    for(int i=0; i<numThreads; ++i)
    {
      auto revhash = createStageFromFunction<Md5Hash, int, reverseHash>();
      revhash->declareActive();

      connect(dist->getNewOutputPort(), revhash->getInputPort());      
      connect(revhash->getOutputPort(), merge->getNewInputPort());      
    }


    connect(producer->getOutputPort(), hash->getInputPort());
    connect(hash->getOutputPort(), dist->getInputPort());
    connect(merge->getOutputPort(), sink->getInputPort());          
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
BENCHMARK(TeeTime_SingleThreaded);

static void TeeTime_MultiThreaded(benchmark::State& state) {
  MultihreadedConfig config(100000, 100, 1000);
  while (state.KeepRunning())
  { 
    config.executeBlocking();
  }
}
BENCHMARK(TeeTime_MultiThreaded);





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


template<typename TPipe>
void foo(benchmark::State& state, TPipe& pipe, int num)
{
  std::atomic<bool> start(false);

  auto produce = [&](){
    while(!start) {
      //do nothing
    }

    const int local_num = num;

    for(int i=0; i<local_num; ++i)
    {
      pipe.add(int(i));
    }
  };

  auto consume = [&](){
    while(!start) {
      //do nothing
    }

    const int local_num = num;

    for(int i=0; i<local_num; ++i)
    {

      while(true)
      {
        auto ret = pipe.removeLast();
        if(ret)
          break;
      }
    }
  }; 
  
  while (state.KeepRunning())
  { 
    std::thread producer(produce);
    std::thread consumer(consume);

    start.store(true);

    producer.join();
    consumer.join();        
  }
}


static void Pipe_Folly(benchmark::State& state) {   

  FollySynchedPipe<int> pipe(1024);
  foo(state, pipe, 1000000);
}

BENCHMARK(Pipe_Folly);

static void Pipe_Locking(benchmark::State& state) {   

  LockingSynchedPipe<int> pipe(1024);
  foo(state, pipe, 1000000);
}

BENCHMARK(Pipe_Locking);


#if 0

struct Settings {
  int num;
  int min;
  int max;
};

static const Settings settings[] = {
  {1000000, 1, 10},
  {1000, 100, 1000},
  {100, 1000, 10000},
};

static void CustomArguments(benchmark::internal::Benchmark* b) {
  for (int i = 0; i < 3; ++i)
    for(int j = 1; j<=8; ++j)
      b->ArgPair(i, j);
}

static void TeeTime_Dist(benchmark::State& state) {
  int numThreads = state.range_y();
  auto s = settings[state.range_x()];

  DistributedConfig config(s.num, s.min, s.max, numThreads);
  while (state.KeepRunning())
  { 
    config.executeBlocking();
  }
}
BENCHMARK(TeeTime_Dist)->Apply(CustomArguments);

#endif

BENCHMARK_MAIN()