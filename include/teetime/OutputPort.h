#pragma once
#include "common.h"
#include "Pipe.h"

namespace teetime
{
  template<typename T>
  class InputPort;

  template<typename T>  
  class OutputPort;

  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input);

  template<typename T>  
  class OutputPort
  {
  public:
    OutputPort()
    : m_pipe(nullptr)
    {      
    }

    OutputPort(const OutputPort&) = delete;

    void send(const T& t) {
      if(m_pipe) {
        m_pipe->add(t);
      }      
    }

  private:  
    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    unique_ptr<Pipe<T>> m_pipe;
  };
}