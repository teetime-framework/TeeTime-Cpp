#pragma once

namespace teetime
{
  template<typename T>
  class Pipe
  {
  public:
    virtual ~Pipe() {}
    virtual T removeLast() = 0;
    virtual void add(const T& t) = 0;
  };
}