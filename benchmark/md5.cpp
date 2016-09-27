#include "Benchmark.h"

using namespace teetime;

void benchmark_teetime(const Params&, int threads);
void benchmark_teetime_prefer_same_cpu(const Params&, int threads);
void benchmark_teetime_avoid_same_core(const Params&, int threads);
void benchmark_fastflow(const Params&, int threads);
void benchmark_fastflow_allocator(const Params&, int threads);

int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  Arguments args(argc, argv);

  const char* name = "md5";
  const char* prettyname = name;
  int num = 0;
  int value = 0;
  int minthreads = 1;
  int maxthreads = 34;

  if (args.contains("fine"))
  {
    num = 70000000;
    value = 2;
    name = "md5_fine";
    prettyname = "MD5 Benchmark, fine grained (360ns)";
  }

  if (args.contains("medium-fine"))
  {
    num = 20000000;
    value = 10;
    name = "md5_medium_fine";
    prettyname = "MD5 Benchmark, medium-fine grained (2us)";
  }

  if (args.contains("medium"))
  {
    num = 10000000;
    value = 20;
    name = "md5_medium";
    prettyname = "MD5 Benchmark, medium grained (3.6us)";
  }

  if (args.contains("coarse"))
  {
    num = 3000000;
    value = 100;
    name = "md5_coarse";
    prettyname = "MD5 Benchmark, coarse grained (18us)";
  }

  num = args.getInt("num", num);
  value = args.getInt("value", value);
  minthreads = args.getInt("minthreads", minthreads);
  maxthreads = args.getInt("threads", maxthreads);

  if (args.contains("fast"))
  {
    num = num / 2;
  }
  else if (args.contains("veryfast"))
  {
    num = num / 4;
  }

  std::cout << "settings: num=" << num << ", value=" << value << ", min threads=" << minthreads << ", max threads=" << maxthreads << std::endl;

  Benchmark benchmark;
  benchmark.setNumValues(num);
  benchmark.setValueRange(value, value);
  benchmark.setThreadRange(minthreads, maxthreads);

  if (!args.contains("noteetime"))
  {
    benchmark.addConfiguration(&benchmark_teetime, "teetime (no affinity)");
    benchmark.addConfiguration(&benchmark_teetime_avoid_same_core, "teetime (avoid same core)");
    benchmark.addConfiguration(&benchmark_teetime_prefer_same_cpu, "teetime (prefer same cpu)");
  }

  if(!args.contains("nofastflow"))
    benchmark.addConfiguration(&benchmark_fastflow, "fastflow (multi alloc)");

  if(!args.contains("nofastflow_alloc"))
    benchmark.addConfiguration(&benchmark_fastflow_allocator, "fastflow (single alloc)");

  benchmark.runAll();
  benchmark.print();
  benchmark.save(name, prettyname);


  return EXIT_SUCCESS;
}
