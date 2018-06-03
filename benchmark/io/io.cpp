#include "../Benchmark.h"

using namespace teetime;

void io_teetime_noAffinity(const Params& params, int threads);
void io_teetime_preferSameCpu(const Params& params, int threads);
void io_teetime_avoidSameCore(const Params& params, int threads);

static void writeFile(const char* filename, const std::vector<char>& writeBuffer, int size)
{
#ifdef TEETIME_USE_FSTREAM
  std::ofstream file;
  file.open(filename, std::ios_base::out | std::ios_base::binary);

  file.write(writeBuffer.data(), size);

  file.close();
#else
  FILE* fp = fopen(filename, "w+b");
  fwrite(writeBuffer.data(), 1, size, fp);
  fclose(fp);
#endif
}

static int readFile(const char* filename, std::vector<char>& readBuffer, int size)
{
#ifdef TEETIME_USE_FSTREAM
  std::ifstream file;
  file.open(filename, std::ios_base::in | std::ios_base::binary);

  file.seekg(0, file.end);
  std::streamsize length = file.tellg();
  file.seekg(0, file.beg);

  assert(length >= 0);
  assert(length == size);

  file.read(readBuffer.data(), length);

  file.close();

  platform::removeFile(filename);
  return static_cast<int>(length);
#else
  FILE* fp = fopen(filename, "rb");
  fseek(fp, 0L, SEEK_END);
  int length = ftell(fp);
  assert(length == size);
  fseek(fp, 0L, SEEK_SET);
  fread(readBuffer.data(), 1, size, fp);
  fclose(fp);

  platform::removeFile(filename);
  return length;
#endif
}

int writeAndReadFile(const char* fileprefix, int fileNum, const std::vector<char>& writeBuffer, std::vector<char>& readBuffer, int size)
{
  char filename[256];
#ifdef __linux__
  sprintf(filename, "/tmp/%s_%d", fileprefix, fileNum);
#else
  sprintf(filename, "%s_%d", fileprefix, fileNum);
#endif

  writeFile(filename, writeBuffer, size);
  int count = readFile(filename, readBuffer, size);

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
    num = 1000000;
    value = 1024;
    name = "io_fine";
    prettyname = "IO Benchmark, fine grain (1,000,000 * 1kb)";
  }
  else if (args.contains("medium-fine"))
  {
    num = 100000;
    value = 1024 * 128;
    name = "io_medium-fine";
    prettyname = "IO Benchmark, medium-fine grain (100,000 * 128kb)";
  }
  else if (args.contains("medium"))
  {
    num = 20000;
    value = 1024 * 1024;
    name = "io_medium";
    prettyname = "IO Benchmark, medium grain (20,000 * 1024Kb)";
  }
  else if (args.contains("coarse"))
  {
    num = 2000;
    value = 1024 * 1024 * 10;
    name = "io_coarse";
    prettyname = "IO Benchmark, coarse grain (2,000 * 10Mb)";
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

  benchmark.runAll();
  benchmark.print();
  benchmark.save(name, prettyname);

  return EXIT_SUCCESS;
}
