#pragma once
#include "Pipe.h"
#include "AbstractStage.h"

namespace teetime
{
  template<typename T>
  class UnsynchedPipe : public Pipe<T>
  {
  public:
    explicit UnsynchedPipe(AbstractStage* targetStage)
     : m_empty(true)
     , m_targetStage(targetStage)
    {      
    }

    virtual T removeLast()
    {
      assert(!m_empty);
      T* t = bufferedValue();
      T value = std::move(*t);
      t->~T();
      m_empty = true;
      return value;
    }

    virtual void add(const T& t)
    {
      //TODO(johl): what to do if pipe non-empty?
      assert(m_empty);
      new (&m_buffer[0]) T(t);
      m_empty = false;

      m_targetStage->executeStage();
    }

  private:
    T* bufferedValue()
    {
      return reinterpret_cast<T*>(&m_buffer[0]);
    }

    char m_buffer[sizeof(T)];
    bool m_empty;

    AbstractStage* m_targetStage;
  };
}