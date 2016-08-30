#include <iostream>
#include <vector>
#include <teetime/logging.h>
#include <teetime/platform.h>

using namespace teetime;

void io_benchmark_teetime(int num, int min, int max, int threads);
void io_benchmark_fastflow(int num, int min, int max, int threads)
{
  unused(num);
  unused(min);
  unused(max);
  unused(threads);
}

struct result
{
  uint64 teetime;
  uint64 fastflow;
};

int main(int argc, char** argv)
{
  if (argc < 5) {
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

  std::vector<result> results;

  for (int i = 1; i <= threads; ++i)
  {
    result res;
    auto start = platform::microSeconds();
    io_benchmark_teetime(num, min, max, i);
    res.teetime = platform::microSeconds() - start;

    start = platform::microSeconds();
    io_benchmark_fastflow(num, min, max, i);
    res.fastflow = platform::microSeconds() - start;

    double speedupTeeTime = 1.0f;
    double speedupFastFlow = 1.0f;
    if (i > 1)
    {
      speedupTeeTime = static_cast<double>(results[0].teetime) / res.teetime;
      speedupFastFlow = static_cast<double>(results[0].fastflow) / res.fastflow;
    }

    std::cout << "threads: " << i << "teetime: " << res.teetime * 0.001 << "ms (" << speedupTeeTime << "),     fastflow: " << res.fastflow * 0.001 << "ms (" << speedupFastFlow << ")" << std::endl;

    results.push_back(res);
  }

  return EXIT_SUCCESS;
}
