#pragma once
#include "AbstractInputPort.h"
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
  class InputPort final : public AbstractInputPort
  {
  public:
    explicit InputPort(AbstractStage* owner)
     : AbstractInputPort(owner)
    {
    }

    InputPort(const InputPort&) = delete;   

    Optional<T> receive() {
      return m_pipe->removeLast();
    }

    virtual void waitForStartSignal() override
    {
      m_pipe->waitForStartSignal();
    }

    virtual bool isClosed() const override
    {
      return m_pipe->isClosed() && m_pipe->isEmpty();
    }

  private:
    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    Pipe<T>* m_pipe;
  };
}