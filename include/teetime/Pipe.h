#pragma once
#include <vector>
#include <mutex>
#include "Signal.h"
#include "Optional.h"
#include <atomic>

namespace teetime
{
  class AbstractPipe
  {
  public:
    AbstractPipe()
    : m_closed(false)
    {      
    }

    virtual ~AbstractPipe() = default;

    virtual void addSignal(const Signal& s) = 0;
    virtual void waitForStartSignal() = 0;

    bool isClosed() const
    {
      return m_closed;
    }

    void close()
    {
      m_closed = true;
    }

  private:
    std::atomic<bool> m_closed;
  };

  template<typename T>
  class Pipe : public AbstractPipe
  {
  public:
    virtual ~Pipe() = default;
    virtual Optional<T> removeLast() = 0;
    virtual void add(const T& t) = 0;
    virtual bool isEmpty() const = 0;
  };
}