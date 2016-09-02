#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <fstream>
#include <sstream>
#include <iterator>
#include <teetime/logging.h>
#include <teetime/platform.h>

using namespace teetime;

void benchmark_teetime(int num, int min, int max, int threads);
void benchmark_teetime_prefer_same_cpu(int num, int min, int max, int threads);
void benchmark_teetime_avoid_same_core(int num, int min, int max, int threads);
void benchmark_fastflow(int num, int min, int max, int threads);
void benchmark_fastflow_allocator(int num, int min, int max, int threads);


class Arguments
{
public:
  Arguments(int argc, char** argv)
    : m_argc(argc)
    , m_argv(argv)
  {}

  const char* getString(const char* name, const char* defaultValue) const
  {
    int index = -1;
    int num = 0;

    find(name, &index, &num);
    if (index > 0 && num > 0)
    {
      return m_argv[index+1];
    }

    return defaultValue;
  }

  int getInt(const char* name, int defaultValue) const
  {
    const char* s = getString(name, nullptr);
    if (s)
    {
      return std::atoi(s);
    }

    return defaultValue;
  }

  bool contains(const char* name) const
  {
    int index = -1;
    int num = 0;

    find(name, &index, &num);

    return (index >= 0);
  }

private:
  void find(const char* name, int* index, int* num) const
  {
    int start = -1;

    for (int i = 1; i < m_argc; ++i)
    {
      if (strlen(m_argv[i]) <= 2)
        continue;

      if (strncmp("--", m_argv[i], 2) != 0)
        continue;

      if (strcmp(name, m_argv[i] + 2) != 0)
        continue;

      start = i;
      break;
    }

    if (index)
      *index = start;

    if (num)
      *num = 0;

    for (int i = start + 1; start > 0 && i < m_argc; ++i)
    {
      if (strncmp("--", m_argv[i], 2) == 0)
        break;

      if (num)
        *num += 1;
    }
  }

  int m_argc;
  char** m_argv;
};





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

  void save(const char* name, const char* prettyname)
  {
    auto filename = nextDataFilename(name);

    if (filename.size() == 0)
    {
      return;
    }

    std::ofstream file;
    file.open(filename, std::ios_base::out);

    for (int i = 0; i < maxThreads; ++i)
    {
      int numThreads = minThreads + i;

      file << numThreads << " ";
      for (int j = 0; j < configs.size(); j++)
      {
        auto& cfg = configs[j];
        assert(cfg.results.size() == maxThreads);
        uint64 baseTime = cfg.results[0].totalTime - cfg.results[0].overheadTime;
        uint64 resultTime = cfg.results[i].totalTime - cfg.results[i].overheadTime;

        file << static_cast<double>(resultTime) * 0.001;
        file << " ";
        file << static_cast<double>(baseTime) / static_cast<double>(resultTime);

        if (j + 1 < configs.size())
        {
          file << " ";
        }
      }

      file << "\n";
    }

    file.close();

    try
    {
      buildAverage(name);
      buildSpeedupGnuPlotScript(name, prettyname);
      buildTimeGnuPlotScript(name, prettyname);
    }
    catch (std::exception& e)
    {
      std::cout << "failed to build average data file: " << e.what() << std::endl;

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

  std::string nextDataFilename(const char* name)
  {
    int suffix = 0;
    while (suffix < 256)
    {
      char filename[256];
      sprintf(filename, "%s_%d.data", name, suffix);

      if (platform::isFile(filename))
      {
        suffix++;
      }
      else
      {
        return filename;
      }
    }

    return std::string();
  }

  void buildAverage(const char* name)
  {
    using Row = std::vector<double>;
    using DataFile = std::vector<Row>;
    int numColumns = -1;
    int numRows = -1;

    DataFile summedDataFiles;
    int numDataFiles = 0;

    int suffix = 0;
    while (suffix < 256)
    {
      char filename[256];
      sprintf(filename, "%s_%d.data", name, suffix);

      if (!platform::isFile(filename))
      {
        break;
      }
      suffix++;

      std::ifstream file;
      file.open(filename, std::ios_base::in);

      DataFile datafile;

      std::string line;
      while (std::getline(file, line))
      {
        Row row;
        std::stringstream ss(line);
        std::istream_iterator<double> myFileIter(ss);
        std::istream_iterator<double> eos;
        std::copy(myFileIter, eos, std::back_inserter(row));

        if (numColumns == -1)
        {
          numColumns = static_cast<int>(row.size());
        }
        else if (numColumns != static_cast<int>(row.size()))
        {
          throw std::logic_error("column number mismatch");
        }

        datafile.push_back(std::move(row));
      }

      if (numRows == -1)
      {
        numRows = static_cast<int>(datafile.size());
      }
      else if (numRows != static_cast<int>(datafile.size()))
      {
        throw std::logic_error("row number mismatch");
      }

      if (summedDataFiles.size() == 0)
      {
        summedDataFiles = datafile;
      }
      else
      {
        for (size_t r = 0; r < summedDataFiles.size(); ++r)
        {
          for (size_t c = 0; c < summedDataFiles[r].size(); ++c)
          {
            summedDataFiles[r][c] += datafile[r][c];
          }
        }
      }
      numDataFiles++;
    }

    std::ofstream ofile;
    ofile.open(std::string(name) + "_avg.data", std::ios_base::out);

    for (size_t r = 0; r < summedDataFiles.size(); ++r)
    {
      for (size_t c = 0; c < summedDataFiles[r].size(); ++c)
      {
        ofile << summedDataFiles[r][c] / numDataFiles << " ";
      }
      ofile << "\n";
    }

    ofile.close();
  }

  void buildSpeedupGnuPlotScript(const char* name, const char* prettyname)
  {
    std::ofstream ofile;
    ofile.open(std::string(name) + "_avg_speedup.gnuplot", std::ios_base::out);

    ofile << "#!/usr/bin/gnuplot" << "\n";
    ofile << "set title '" << prettyname << "'" << "\n";
    ofile << "set terminal png size 800,600 enhanced font \"Helvetica, 12\"" << "\n";
    ofile << "set output '" << name << "_avg_speedup.png'" << "\n";

    ofile << "set xrange [" << (minThreads - 1) << ":" << maxThreads << "]" << "\n";
    ofile << "set yrange [" << (minThreads - 1) << ":" << maxThreads << "]" << "\n";

    ofile << "set grid ytics lc rgb \"#bbbbbb\" lw 1 lt 0" << "\n";
    ofile << "set grid xtics lc rgb \"#bbbbbb\" lw 1 lt 0" << "\n";

    ofile << "set xtics 2" << "\n";
    ofile << "set ytics 2" << "\n";

    ofile << "set xlabel \"number of threads\"" << "\n";
    ofile << "set ylabel \"speedup\"" << "\n";

    ofile << "set key left top" << "\n";

    ofile << "set datafile separator \" \"" << "\n";

    ofile << "plot \"" << name << "_avg.data\" using 1:1 with lines linetype 1 title \"Ideal\"";
    int i = 1;
    for (auto& config : configs)
    {
      ofile << ",\\" << "\n";
      ofile << "     \"\" using 1:" << i*2 + 1 << " with lines linetype " << i+1 << " title \"" << config.name << "\"";
      i++;
    }
  }

  void buildTimeGnuPlotScript(const char* name, const char* prettyname)
  {
    std::ofstream ofile;
    ofile.open(std::string(name) + "_avg_time.gnuplot", std::ios_base::out);

    ofile << "#!/usr/bin/gnuplot" << "\n";
    ofile << "set title '" << prettyname << "'" << "\n";
    ofile << "set terminal png size 800,600 enhanced font \"Helvetica, 12\"" << "\n";
    ofile << "set output '" << name << "_avg_time.png'" << "\n";

    ofile << "set xrange [" << minThreads << ":" << maxThreads << "]" << "\n";
    ofile << "set autoscale y" << "\n";

    ofile << "set grid ytics lc rgb \"#bbbbbb\" lw 1 lt 0" << "\n";
    ofile << "set grid xtics lc rgb \"#bbbbbb\" lw 1 lt 0" << "\n";

    ofile << "set xlabel \"number of threads\"" << "\n";
    ofile << "set ylabel \"time (milliseconds)\"" << "\n";

    ofile << "set key right top" << "\n";

    ofile << "set datafile separator \" \"" << "\n";

    for (size_t i = 0; i < configs.size(); ++i)
    {
      if (i == 0)
      {
        ofile << "plot ";//\"" << name << "_avg.data\""
      }
      else
      {
        ofile << ",\\\n";
      }

      ofile << " \"" << name << "_avg.data\" using 1:" << ((i + 1) * 2) << " with lines linetype " << i + 2 << " title \"" << configs[i].name << "\"";
    }
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
