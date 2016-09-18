#include "../Benchmark.h"
#include <teetime/Image.h>

using namespace teetime;

void mipmaps_teetime_noAffinity(const Params& params, int threads);
void mipmaps_teetime_preferSameCpu(const Params& params, int threads);
void mipmaps_teetime_avoidSameCore(const Params& params, int threads);
void mipmaps_fastflow(const Params& params, int threads);
void mipmaps_fastflow_allocator(const Params& params, int threads);


std::string getImageInputDirectory(int num, int size)
{
  char pwd[256];
  platform::getCurrentWorkingDirectory(pwd, sizeof(pwd));

  char dirname[256];
  sprintf(dirname, "%s/input_images_%d_%d", pwd, num, size);

  return dirname;
}

std::string getImageOutputDirectory()
{
  char pwd[256];
  platform::getCurrentWorkingDirectory(pwd, sizeof(pwd));

  char dirname[256];
  sprintf(dirname, "%s/output_images", pwd);

  return dirname;
}

std::string createInputData(int num, int size)
{
  auto dirname = getImageInputDirectory(num, size);

  if (!platform::isDirectory(dirname.c_str()))
  {
    if (!platform::createDirectory(dirname.c_str()))
      throw std::logic_error("failed to create ...");

    if(!platform::isFile(TEETIME_MIPMAP_IMAGE))
      throw std::logic_error("failed to load test image ...");

    Image image;
    if(!image.loadFromFile(TEETIME_MIPMAP_IMAGE))
      throw std::logic_error("failed to load test image ...");

    size_t imagesize = (size_t(1) << size);
    image = image.resize(imagesize, imagesize);

    for (int i = 0; i < num; ++i)
    {
      char filename[256];
      sprintf(filename, "%s/lena%d.png", dirname.c_str(), i);

      image.saveToPngFile(filename);
    }
  }

  return dirname;
}

class MipmapBenchmark : public Benchmark
{
public:

private:
  virtual void setup(const Params& params) override
  {
    auto s = getImageOutputDirectory();
    std::vector<std::string> files;
    if (platform::listFiles(s, files, false))
    {
      for (const auto& f : files)
      {
        platform::removeFile(s + "/" + f);
      }
    }
    else
    {
      platform::createDirectory(s);
    }
  }

  virtual void teardown(const Params& params) override
  {
  }
};

int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));

  Arguments args(argc, argv);

  const char* name = "mipmap";
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
    num = 10000;
    value = 1024 * 1024;
    name = "io_medium";
    prettyname = "IO Benchmark, medium grain (10,000 * 1024Kb)";
  }
  else if (args.contains("coarse"))
  {
    num = 1000;
    value = 1024 * 1024 * 10;
    name = "io_coarse";
    prettyname = "IO Benchmark, coarse grain (1,000 * 10Mb)";
  }

  num = args.getInt("num", num);
  value = args.getInt("value", value);
  minthreads = args.getInt("minthreads", minthreads);
  maxthreads = args.getInt("maxthreads", maxthreads);

  std::cout << "settings: num=" << num << ", value=" << value << ", min threads=" << minthreads << ", max threads=" << maxthreads << std::endl;

  auto s = createInputData(num, value);

  MipmapBenchmark benchmark;
  benchmark.setNumValues(num);
  benchmark.setValueRange(value, value);
  benchmark.setThreadRange(minthreads, maxthreads);

  if(!args.contains("noteetime"))
    benchmark.addConfiguration(mipmaps_teetime_noAffinity, "teetime (no affinity)");
#if 0
  if(!args.contains("noteetime_sameCPU"))
    benchmark.addConfiguration(io_teetime_preferSameCpu, "teetime (prefer same CPU)");

  if(!args.contains("noteetime_sameCore"))
    benchmark.addConfiguration(io_teetime_avoidSameCore, "teetime (avoid same core)");

  if(!args.contains("nofastflow"))
    benchmark.addConfiguration(io_fastflow, "fastflow (multi alloc)");

  if(!args.contains("nofastflow_alloc"))
    benchmark.addConfiguration(io_fastflow_allocator, "fastflow (single alloc)");
#endif
  benchmark.runAll();
  benchmark.print();
  benchmark.save(name, prettyname);

  return EXIT_SUCCESS;
}
