#include "Benchmark.h"

using namespace teetime;

void io_teetime_noAffinity(int num, int min, int max, int threads);
void io_teetime_preferSameCpu(int num, int min, int max, int threads);
void io_teetime_avoidSameCore(int num, int min, int max, int threads);
void io_fastflow(int num, int min, int max, int threads);
void io_fastflow_allocator(int num, int min, int max, int threads);

static void writeFile(const char* filename, int value)
{
  std::ofstream file;
  file.open(filename, std::ios_base::out | std::ios_base::binary);

  const uint8 b = static_cast<uint8>(value % 256);
  for (int i = 0; i < value; ++i)
  {
    file.write(reinterpret_cast<const char*>(&b), 1);
  }
  file.close();
}

static int readFile(const char* filename)
{
  std::ifstream file;
  file.open(filename, std::ios_base::in | std::ios_base::binary);

  int count = 0;
  while (true)
  {
    uint8 b;
    file.read(reinterpret_cast<char*>(&b), 1);

    if (file.eof())
      break;
    else
      count += 1;
  }
  file.close();

  platform::removeFile(filename);
  return count;
}

int writeAndReadFile(const char* fileprefix, int fileNum, int size)
{
  char filename[256];
  sprintf(filename, "%s_%d", fileprefix, fileNum);

  writeFile(filename, size);
  int count = readFile(filename);

  assert(count == size);
  return count;
}

int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  Arguments args(argc, argv);

  const char* name = "io";
  const char* prettyname = name;
  int num = 0;
  int value = 0;
  int minthreads = 1;
  int maxthreads = 34;

  if (args.contains("fine"))
  {
    num = 100000;
    value = 1024; 
    name = "io_fine";
    prettyname = "IO Benchmark, fine grain (100,000 * 1kb)";
  }
  else if (args.contains("medium-fine"))
  {
    num = 100000;
    value = 4096;
    name = "io_medium-fine";
    prettyname = "IO Benchmark, medium-fine grain (100,000 * 4kb)";
  }
  else if (args.contains("medium"))
  {
    num = 1000;
    value = 1024 * 256;
    name = "io_medium";
    prettyname = "IO Benchmark, medium grain (1,000 * 256kb)";
  }
  else if (args.contains("coarse"))
  {
    num = 100;
    value = 1024 * 1024;
    name = "io_coarse";
    prettyname = "IO Benchmark, coarse grain (100 * 1Mb)";
  }

  num = args.getInt("num", num);
  value = args.getInt("value", value);
  minthreads = args.getInt("minthreads", minthreads);
  maxthreads = args.getInt("threads", maxthreads);

  std::cout << "settings: num=" << num << ", value=" << value << ", min threads=" << minthreads << ", max threads=" << maxthreads << std::endl;

  Benchmark benchmark;
  benchmark.setNumValues(num);
  benchmark.setValueRange(value, value);
  benchmark.setThreadRange(minthreads, maxthreads);
  benchmark.addConfiguration(io_teetime_noAffinity, "teetime (no affinity)");
  benchmark.addConfiguration(io_teetime_preferSameCpu, "teetime (prefer same CPU)");
  benchmark.addConfiguration(io_teetime_avoidSameCore, "teetime (avoid same core)");
  benchmark.addConfiguration(io_fastflow, "fastflow (multi alloc)");
  benchmark.addConfiguration(io_fastflow_allocator, "fastflow (single alloc)");
  benchmark.runAll();
  benchmark.print();
  benchmark.save(name, prettyname);

  return EXIT_SUCCESS;
}
