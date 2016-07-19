#include <benchmark/benchmark.h>
#include <teetime/stages/RandomIntProducer.h>
#include <teetime/stages/FunctionStage.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/benchmark/ReverseIntMd5Hashing.h>
#include <teetime/ports/Port.h>
#include <teetime/Configuration.h>
#include <teetime/Md5Hash.h>
#include <climits>

using namespace teetime;

namespace
{
  Md5Hash int2hash(int i)
  {
    return Md5Hash::generate(&i, sizeof(i));
  }

  class Md5BenchmarkConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<int>> ints;

    explicit Md5BenchmarkConfig()
    {
      auto producer = createStage<RandomIntProducer>(0, 10000, 100);
      auto md5 = createStage<FunctionStage<int, Md5Hash, int2hash>>();
      auto crackMd5 = createStage<ReverseIntMd5Hashing>();
      ints = createStage<CollectorSink<int>>(100);

      producer->declareActive();
      connect(producer->getOutputPort(), md5->getInputPort());
      connect(md5->getOutputPort(), crackMd5->getInputPort());
      connect(crackMd5->getOutputPort(), ints->getInputPort());
    }
  };
}




// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {  
  while (state.KeepRunning())
  {
    Md5BenchmarkConfig config;
    config.executeBlocking();
  }
}
BENCHMARK(BM_StringCopy);

BENCHMARK_MAIN()