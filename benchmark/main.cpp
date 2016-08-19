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


template<typename TPipe>
void foo(benchmark::State& state, TPipe& pipe, int num)
{
  std::atomic<bool> start(false);
  std::atomic<int> ready(0);

  std::vector<int> produced;
  std::vector<int> consumed;

  produced.reserve(num);
  consumed.reserve(num);

  auto produce = [&](){

    ready.fetch_add(1);
    
    //std::cout << "producer ready: " << ready.load() << std::endl;

    while(!start) {
      //do nothing
    }

    const int local_num = num;

    for(int i=0; i<local_num; ++i)
    {
      //produced.push_back(i);
      pipe.add(int(i));
    }
  };

  auto consume = [&](){

    ready.fetch_add(1);

    //std::cout << "consumer ready: " << ready.load() << std::endl;

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
        {
          //consumed.push_back(*ret);
          break;
        }
      }
    }
  }; 


  while (state.KeepRunning())
  { 
    produced.clear();
    consumed.clear();
    ready.store(0);

    std::thread producer(produce);
    std::thread consumer(consume);

    while(ready.load() != 2)
    {
      //std::cout << "main ready: " << ready.load() << std::endl;
    }

    start.store(true);

    producer.join();
    consumer.join(); 
  }

  if(produced.size() != consumed.size())
  {
    std::cout << "consumed.size() != produced.size()" << std::endl;
  }
  else if(memcmp(produced.data(), consumed.data(), consumed.size() * sizeof(int)) != 0)
  {
    std::cout << "consumed != produced" << std::endl;
  }
}



template<typename T>
class MyQueue
{
public:

  explicit MyQueue(unsigned capacity)
   : m_array(new Entry[capacity])
   , m_capacity(capacity)
   , m_readIndex(0)
   , m_writeIndex(0)
  {}

  ~MyQueue()
  {
    delete m_array;
  }

  bool tryAdd(const T& t)
  {
    auto& entry = m_array[m_writeIndex];

    if(!entry.hasValue.load(std::memory_order_acquire))
    {
      new (&entry.data[0]) T(t);
      entry.hasValue.store(true, std::memory_order_release);

      if(++m_writeIndex == m_capacity)
      {
        m_writeIndex = 0;
      }
      return true;
    }

    return false;
  }

  void add(const T& t)
  {
    for(;;)
    {
      if(tryAdd(t))
      {
        break;
      }
    }
  }

  Optional<T> removeLast()
  {
    auto& entry = m_array[m_readIndex];

    Optional<T> ret;

    if(entry.hasValue.load(std::memory_order_acquire))
    {
      ret.set(std::move(*reinterpret_cast<T*>(&entry.data[0])));
      entry.hasValue.store(false, std::memory_order_release);

      if(++m_readIndex == m_capacity)
      {
        m_readIndex = 0;
      }      
    }

    return ret;
  }


private:
  struct Entry
  {
    Entry()
    {
      hasValue.store(false);
    }

    std::atomic<bool> hasValue;
    char data[sizeof(T)];
  };

  char padding1_[64];
  unsigned m_readIndex;
  char padding2_[64];
  unsigned m_writeIndex;
  char padding3_[64];  

  Entry* m_array;
  unsigned m_capacity;
};



template<typename T>
class MyQueue2
{
public:

  explicit MyQueue2(unsigned capacity)
   : m_array(new Entry[capacity])
   , m_capacity(capacity)
   , m_readIndex(0)
   , m_writeIndex(0)
  {}

  ~MyQueue2()
  {
    delete m_array;
  }

  bool tryAdd(const T& t)
  {
    auto& entry = m_array[m_writeIndex];

    if(!entry.hasValue.load(std::memory_order_acquire))
    {
      new (&entry.data[0]) T(t);
      entry.hasValue.store(true, std::memory_order_release);

      if(++m_writeIndex == m_capacity)
      {
        m_writeIndex = 0;
      }
      return true;
    }

    return false;
  }

  void add(const T& t)
  {
    for(;;)
    {
      if(tryAdd(t))
      {
        break;
      }
    }
  }

  Optional<T> removeLast()
  {
    auto& entry = m_array[m_readIndex];

    Optional<T> ret;

    if(entry.hasValue.load(std::memory_order_acquire))
    {
      ret.set(std::move(*reinterpret_cast<T*>(&entry.data[0])));
      entry.hasValue.store(false, std::memory_order_release);

      if(++m_readIndex == m_capacity)
      {
        m_readIndex = 0;
      }      
    }

    return ret;
  }


private:
  struct Entry
  {
    Entry()
    {
      hasValue.store(false);
    }

    std::atomic<bool> hasValue;
    char data[sizeof(T)];
  };

  static const int cacheLineSize = 64;
#if 0
  int foo alignas(cacheLineSize);
  unsigned m_readIndex alignas(cacheLineSize);
  unsigned m_writeIndex alignas(cacheLineSize);;
  Entry* m_array alignas(cacheLineSize);
  unsigned m_capacity alignas(cacheLineSize);
#else
  char padding1_[64];
  unsigned m_readIndex;
  char padding2_[64];
  unsigned m_writeIndex;
  char padding3_[64];  

  Entry* m_array;
  unsigned m_capacity;
#endif
  
  
};

static const int queuetestnum = 10000;

static void Pipe_Empty(benchmark::State& state) {   

  FollySynchedPipe<int> pipe(1024);
  foo(state, pipe, 0);
}

BENCHMARK(Pipe_Empty);

static void Pipe_Folly(benchmark::State& state) {   

  FollySynchedPipe<int> pipe(1024);
  foo(state, pipe, queuetestnum);
}

BENCHMARK(Pipe_Folly);

static void Pipe_Locking(benchmark::State& state) {   

  LockingSynchedPipe<int> pipe(1024);
  foo(state, pipe, queuetestnum);
}

//BENCHMARK(Pipe_Locking);

static void Pipe_MyQueue(benchmark::State& state) {   

  MyQueue<int> pipe(1024);
  foo(state, pipe, queuetestnum);
}

BENCHMARK(Pipe_MyQueue);


static void Pipe_MyQueue2(benchmark::State& state) {   

  MyQueue2<int> pipe(1024);
  foo(state, pipe, queuetestnum);
}

BENCHMARK(Pipe_MyQueue2);


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