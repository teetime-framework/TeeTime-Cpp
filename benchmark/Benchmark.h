/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
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

namespace teetime
{
  static const std::vector<int> affinity_none = {
    -1
  };

  static const std::vector<int> affinity_avoidSameCore = {
    0,1,2,3,4,5,6,7,
    8,9,10,11,12,13,14,15,
    16,17,18,19,20,21,22,23,
    24,25,26,27,28,29,30,31
  };

  static const std::vector<int> affinity_preferSameCpu = {
    0,1,2,3,4,5,6,7,
    16,17,18,19,20,21,22,23,
    8,9,10,11,12,13,14,15,
    24,25,26,27,28,29,30,31
  };

  class CpuDispenser
  {
  public:
    explicit CpuDispenser(const std::vector<int>& affinity)
      : m_affinity(affinity)
      , m_next(0)
    {}

    int next()
    {
      int cpu = m_affinity[m_next++ % m_affinity.size()];

      if (cpu >= static_cast<int>(std::thread::hardware_concurrency()))
        return -1;

      return cpu;
    }

  private:    
    std::vector<int> m_affinity;
    int m_next;
  };


  class Params
  {
  public:
    Params() = default;
    ~Params() = default;
    Params(const Params&) = default;
    Params& operator=(const Params&) = default;

    void set(const char* name, const char* value)
    {
      entry(name, true)->value = value;
    }

    void set(const char* name, int i)
    {
      char buffer[256];
      sprintf(buffer, "%d", i);
      set(name, buffer);
    }

    void set(const char* name, float f)
    {
      char buffer[256];
      sprintf(buffer, "%f", f);
      set(name, buffer);
    }

    const char* getString(const char* name) const
    {
      if (auto e = entry(name))
      {
        return e->value.c_str();
      }

      return "";
    }

    int getInt32(const char* name) const
    {
      return std::atoi(getString(name));
    }

  private:
    struct Entry {
      std::string name;
      std::string value;
    };

    const Entry* entry(const char* name) const
    {
      for (const auto& e : m_entries)
      {
        if (e.name == name)
        {
          return &e;
        }
      }

      return nullptr;
    }

    Entry* entry(const char* name, bool create)
    {
      for (auto& e : m_entries)
      {
        if (e.name == name)
        {
          return &e;
        }
      }

      if (create)
      {
        m_entries.push_back(Entry{ name, "" });
        return &m_entries[m_entries.size() - 1];
      }

      return nullptr;
    }

    std::vector<Entry> m_entries;
  };

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
        return m_argv[index + 1];
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
    using Function = void(const Params& params, int threads);

    void setNumValues(int num)
    {
      m_params.set("num", num);
    }

    void setValueRange(int min, int max)
    {
      m_params.set("minvalue", min);
      m_params.set("maxvalue", max);
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
      for (int i = 0; (minThreads + i) < maxThreads; ++i)
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
    virtual void setup(const Params& params)
    {
      unused(params);
    }

    virtual void teardown(const Params& params)
    {
      unused(params);
    }

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
      setup(m_params);
      Result res;
      res.overheadTime = overhead(cfg, numThreads, 10);

      auto start = platform::microSeconds();
      cfg.f(m_params, numThreads);
      res.totalTime = platform::microSeconds() - start;

      std::cout << res.totalTime * 0.001 << "ms (overhead: " << res.overheadTime * 0.001 << "ms)";

      cfg.results.push_back(res);
      teardown(m_params);
    }

    uint64 overhead(Config& cfg, int numThreads, int iterations)
    {
#if 1
      unused(cfg);
      unused(numThreads);
      unused(iterations);
      return 0;
#else
      uint64 overhead = 0;

      for (int i = 0; i < iterations; ++i)
      {
        auto start = platform::microSeconds();
        cfg.f(m_params, numThreads);
        overhead += (platform::microSeconds() - start);
      }

      return overhead / iterations;
#endif
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
        ofile << "     \"\" using 1:" << i * 2 + 1 << " with lines linetype " << i + 1 << " title \"" << config.name << "\"";
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
    Params m_params;
    int minThreads;
    int maxThreads;
  };

}