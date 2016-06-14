#pragma once
#include <vector>
#include <mutex>
#include "Signal.h"
#include "Optional.h"

namespace teetime
{
  class AbstractPipe
  {
  public:
    virtual ~AbstractPipe() = default;

    virtual void addSignal(const Signal& s) = 0;
    virtual void waitForStartSignal() = 0;
  };

  template<typename T>
  class Pipe : public AbstractPipe
  {
  public:
    virtual ~Pipe() = default;
    virtual Optional<T> removeLast() = 0;
    virtual void add(const T& t) = 0;
  };
}