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

  class AbstractInputPort
  {
  public:
    virtual ~AbstractInputPort() = default;

    virtual void waitForStartSignal() = 0;
  };

  template<typename T>  
  class InputPort : public AbstractInputPort
  {
  public:
    explicit InputPort(AbstractStage* owner)
     : m_owner(owner)
    {
      assert(m_owner);
      m_owner->registerPort(this);
    }

    InputPort(const InputPort&) = delete;   

    Optional<T> receive() {
      return m_pipe->removeLast();
    }

    AbstractStage* getOwningStage()
    {
      return m_owner;
    }

    virtual void waitForStartSignal() override
    {
      m_pipe->waitForStartSignal();
    }

  private:
    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    Pipe<T>* m_pipe;
    AbstractStage* m_owner;
  };
}