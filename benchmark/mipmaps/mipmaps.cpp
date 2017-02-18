#include "../Benchmark.h"
#include <teetime/Image.h>

using namespace teetime;

void mipmaps_teetime_noAffinity(const Params& params, int threads);
void mipmaps_teetime_preferSameCpu(const Params& params, int threads);
void mipmaps_teetime_avoidSameCore(const Params& params, int threads);

std::string getImageInputDirectory(int num, int size)
{
  char dirname[256];
#ifdef __linux__  
  sprintf(dirname, "%s/input_images_%d_%d", "/tmp", num, size);
#else
  char pwd[256];
  platform::getCurrentWorkingDirectory(pwd, sizeof(pwd));

  sprintf(dirname, "%s/input_images_%d_%d", pwd, num, size);
#endif

  return dirname;
}

std::string getImageOutputDirectory()
{
  char dirname[256];
#ifdef __linux__  
  sprintf(dirname, "%s/output_images", "/tmp");
#else
  char pwd[256];
  platform::getCurrentWorkingDirectory(pwd, sizeof(pwd));

  sprintf(dirname, "%s/output_images", pwd);
#endif

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

  const char* name = "mipmaps";
  const char* prettyname = name;
  int num = 0;
  int value = 0;
  int minthreads = 1;
  int maxthreads = 34;

  if (args.contains("fine"))
  {
    num = 2000;
    value = 6; 
    name = "mipmaps_fine";
    prettyname = "Mipmaps Benchmark, fine grain (1,000,000 * 1kb)";
  }
  else if (args.contains("medium-fine"))
  {
    num = 200;
    value = 8;
    name = "mipmaps_medium-fine";
    prettyname = "Mipmaps Benchmark, medium-fine grain (100,000 * 128kb)";
  }
  else if (args.contains("medium"))
  {
    num = 100;
    value = 9;
    name = "mipmaps_medium";
    prettyname = "Mipmaps Benchmark, medium grain (10,000 * 1024Kb)";
  }
  else if (args.contains("coarse"))
  {
    num = 20;
    value = 10;
    name = "mipmaps_coarse";
    prettyname = "Mipmaps Benchmark, coarse grain (1,000 * 10Mb)";
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

  benchmark.addConfiguration(mipmaps_teetime_noAffinity, "teetime (no affinity)");
  benchmark.addConfiguration(mipmaps_teetime_preferSameCpu, "teetime (prefer same CPU)");
  benchmark.addConfiguration(mipmaps_teetime_avoidSameCore, "teetime (avoid same core)");

  benchmark.runAll();
  benchmark.print();
  benchmark.save(name, prettyname);

  return EXIT_SUCCESS;
}
