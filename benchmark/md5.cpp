#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <teetime/logging.h>
#include <teetime/platform.h>

using namespace teetime;

void benchmark_teetime(int num, int min, int max, int threads);
void benchmark_fastflow(int num, int min, int max, int threads);
void benchmark_fastflow_allocator(int num, int min, int max, int threads);

class Benchmark
{
public:
  using Function = void(int num, int min, int max, int threads);

  void setNumValues(int num)
  {
    numValues = num;
  }

  void setValueRange(int min, int max)
  {
    minValue = min;
    maxValue = max;
  }

  void setThreadRange(int min, int max)
  {
    minThreads = min;
    maxThreads = max;
  }

  void addConfiguration(Function* f, const char* name)
  {
    Config cfg;
    cfg.f = f;
    cfg.name = name;

    configs.push_back(cfg);
  }

  void runAll()
  {
    for (int numThreads = minThreads; numThreads <= maxThreads; ++numThreads)
    {
      for (auto& cfg : configs)
      {
        std::cout << "running '" << cfg.name << "' with " << numThreads << " threads...";
        std::cout.flush();
        run(cfg, numThreads);
        std::cout << std::endl;
      }
    }
  }

  void print()
  {
    for ( int i = 0; i < maxThreads; ++i)
    {
      int numThreads = minThreads + i;

      std::cout << "threads=" << numThreads;
      for (auto& cfg : configs)
      { 
        assert(cfg.results.size() == maxThreads);
        uint64 baseTime = cfg.results[0].totalTime - cfg.results[0].overheadTime;
        uint64 resultTime = cfg.results[i].totalTime - cfg.results[i].overheadTime;

        std::cout << "  " << cfg.name << ": " << resultTime * 0.001 << "ms ";
        std::cout << "(s=" << static_cast<float>(baseTime) / static_cast<float>(resultTime) << ")";
      }

      std::cout << std::endl;
    }
  }

private:
  struct Result
  {
    int threadNum;
    uint64 totalTime;
    uint64 overheadTime;
  };

  struct Config
  {
    Function* f;
    std::string name;
    std::vector<Result> results;
  };

  void run(Config& cfg, int numThreads)
  {
    Result res;
    res.overheadTime = overhead(cfg, numThreads, 10);

    auto start = platform::microSeconds();
    cfg.f(numValues, minValue, maxValue, numThreads);
    res.totalTime = platform::microSeconds() - start;

    std::cout << res.totalTime * 0.001 << "ms (overhead: " << res.overheadTime * 0.001 << "ms)";

    cfg.results.push_back(res);
  }

  uint64 overhead(Config& cfg, int numThreads, int iterations)
  {
    uint64 overhead = 0;

    for (int i = 0; i < iterations; ++i)
    {
      auto start = platform::microSeconds();
      cfg.f(0, minValue, maxValue, numThreads);
      overhead += (platform::microSeconds() - start);
    }

    return overhead / iterations;
  }

  std::vector<Config> configs;
  int numValues;
  int minValue;
  int maxValue;
  int minThreads;
  int maxThreads;
};

int main(int argc, char** argv)
{
  if (argc < 5) {
    std::cout << "usage: md5 <count> <min> <max> <threads>" << std::endl;
    return EXIT_FAILURE;
  }

  bool no_teetime = false;
  bool no_fastflow = false;
  bool no_fastflow_alloc = false;

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "nofastflow") == 0)
      no_fastflow = true;
    else if (strcmp(argv[i], "noteetime") == 0)
      no_teetime = true;
    else if (strcmp(argv[i], "nofastflow_alloc") == 0)
      no_fastflow_alloc = true;
  }

  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  int num = atoi(argv[1]);
  int min = atoi(argv[2]);
  int max = atoi(argv[3]);
  int threads = atoi(argv[4]);

  std::cout << "settings: num=" << num << ", min=" << min << ", max=" << max << ", threads=" << threads << std::endl;


  Benchmark benchmark;
  benchmark.setNumValues(num);
  benchmark.setValueRange(min, max);
  benchmark.setThreadRange(1, threads);

  if(!no_teetime)
    benchmark.addConfiguration(&benchmark_teetime, "teetime");

  if(!no_fastflow)
    benchmark.addConfiguration(&benchmark_fastflow, "fastflow");

  if(!no_fastflow_alloc)
    benchmark.addConfiguration(&benchmark_fastflow_allocator, "fastflow (allocator)");

  benchmark.runAll();
  benchmark.print();

  return EXIT_SUCCESS;
}
