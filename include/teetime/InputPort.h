#pragma once
#include "Pipe.h"

namespace teetime
{
  template<typename T>
  class InputPort;

  template<typename T>  
  class OutputPort;

  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input);

  class AbstractStage;

  template<typename T>  
  class InputPort
  {
  public:
    explicit InputPort(AbstractStage* owner)
     : m_owner(owner)
    {}

    InputPort(const InputPort&) = delete;   

    T receive() {
      return m_pipe->removeLast();
    }

    AbstractStage* getOwningStage()
    {
      return m_owner;
    }

  private:
    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    Pipe<T>* m_pipe;
    AbstractStage* m_owner;
  };
}