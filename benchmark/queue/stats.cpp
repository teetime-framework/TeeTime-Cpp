#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>


int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "no filename" << std::endl;
    return EXIT_FAILURE;
  }

  const char* filename = argv[1];

  std::ifstream file;
  file.open(filename, std::ios::binary | std::ios::in);
  if (!file.is_open()) {
    std::cout << "file not found: " << filename << std::endl;
    return EXIT_FAILURE;
  }

  std::string line;

  std::vector<double> values;

  double sum = 0;
  double count = 0;
  while (std::getline(file, line))
  {
    double v = ::atof(line.c_str());
    values.push_back(v);
    sum += v;
    count += 1;
  }

  std::sort(values.begin(), values.end());
  std::cout << "#samples: " << count << std::endl;
  std::cout << "Average: " << sum / count << std::endl;
  std::cout << "Median: " << values[values.size() / 2] << std::endl;
  std::cout << "Minimum: " << values[0] << std::endl;
  std::cout << "Maximum: " << values[values.size() - 1] << std::endl;
}