#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <teetime/platform.h>
#include <cstring>

void getDirectory(const char* path, std::string& directory, std::string& file)
{
  assert(path);

  size_t len = strlen(path);

  if (len == 0)
    return;

  for (size_t i = 0; i < len; ++i)
  {
    char c = path[len - (i + 1)];
    if (c == '/' || c == '\\')
    {
      directory = std::string(path, len - (i + 1));
      file = std::string(path + len - i);
      return;
    }
  }
}

bool startsWith(const std::string& s, const std::string& prefix)
{
  if (s.size() >= prefix.size())
  {
    return std::strncmp(s.c_str(), prefix.c_str(), prefix.size()) == 0;
  }

  return false;
}

bool endsWith(const std::string& s, const std::string& suffix)
{
  if (s.size() >= suffix.size())
  {
    return std::strncmp(s.c_str() + (s.size() - suffix.length()), suffix.c_str(), suffix.size()) == 0;
  }

  return false;
}

using Pass = std::vector<double>;

std::vector<Pass> readFile(const std::string& filename)
{
  std::vector<Pass> ret;

  std::ifstream file;
  file.open(filename, std::ios_base::in);

  std::string line;
  while (std::getline(file, line))
  {
    std::stringstream ss(line);
    Pass pass;
    double d;
    while (ss >> d)
    {
      pass.push_back(d);
    }

    ret.push_back(std::move(pass));
  }

  return ret;
}

Pass buildAverage(const std::vector<Pass>& passes)
{
  Pass ret;

  for (size_t threads = 0; threads < passes[0].size(); ++threads)
  {
    double time = 0;
    
    for (const auto& pass : passes)
    {
      assert(pass.size() > threads);
      time += pass[threads];
    }

    ret.push_back(time / passes.size());
  }

  return ret;
}

Pass buildSpeedup(const Pass& p)
{
  Pass ret;

  for (auto t : p)
  {
    ret.push_back(p[0] / t);
  }

  return ret;
}


void writeData(const std::string& filename, const std::vector<std::string>& data)
{
  std::ofstream file;
  file.open(filename, std::ios::out);

  for (const auto& s : data)
  {
    file << s << "\n";
  }

  file.close();
}

int main(int argc, char** argv)
{
  std::string dir, file;
  getDirectory(argv[1], dir, file);
  std::vector<std::string> files;
  teetime::platform::listFiles(dir, files, false);

  static const std::string suffix = ".passes";
  
  std::vector<std::string> cfg_files;

  for (const auto& s : files)
  {
    if (startsWith(s, file) && endsWith(s, suffix))
    {
      cfg_files.push_back(s);
    }
  }

  std::sort(cfg_files.begin(), cfg_files.end());

  std::vector<std::string> lines;

  for (size_t i=0; i<cfg_files.size(); ++i)
  {
    const std::string& s = cfg_files[i];

    std::string cfg_name = std::string(s.c_str() + file.size(), s.size() - (file.size() + suffix.size()));
    std::cout << "file found: " << s << " (" << cfg_name << ")\n";

    auto passes = readFile(s);
    auto avg = buildAverage(passes);
    auto speedup = buildSpeedup(avg);

    while (lines.size() < (speedup.size() + 1))
      lines.push_back("");

    lines[0] += "# ";
    lines[0] += s;

    for (size_t j=0; j<speedup.size(); ++j)
    {
      char buffer[256];

      if (lines[j + 1].size() == 0)
      {
        sprintf(buffer, "%d ", (j+1));
        lines[j + 1] = std::string(buffer);
      }

      sprintf(buffer, "%f ", avg[j]);
      lines[j + 1] += std::string(buffer);

      sprintf(buffer, "%f ", speedup[j]);
      lines[j + 1] += std::string(buffer);
    }
  }

  const std::string dataFilename = std::string(argv[1]) + ".data";  

  writeData(dataFilename, lines); 

  return EXIT_SUCCESS;
}