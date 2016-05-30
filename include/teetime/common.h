#pragma once
#include <cassert>
#include <memory>
#include <iostream>

namespace teetime
{
  inline void log(const char* file, int line, const char* message)
  {
    std::cout << file << "(" << line << "): " << message << std::endl;
  }

  using std::shared_ptr;
  using std::unique_ptr;
}