#pragma once
#include "common.h"
#include "Pipe.h"
#include "Signal.h"

namespace teetime
{
  template<typename T>
  class InputPort;

  template<typename T>  
  class OutputPort;

  template<typename T>
  void connect(OutputPort<T>& output, InputPort<T>& input);

  class AbstractOutputPort
  {
  public:
    virtual ~AbstractOutputPort() = default;

    void sendSignal(const Signal& signal)
    {
      if(auto p = getPipe()) {
        p->addSignal(signal);
      }
    }

  private:
    virtual AbstractPipe* getPipe() = 0;
  };

  template<typename T>  
  class OutputPort : public AbstractOutputPort
  {
  public:
    explicit OutputPort(AbstractStage* owner)
    : m_pipe(nullptr)
    {     
      assert(owner);
    }

    OutputPort(const OutputPort&) = delete;

    void send(const T& t) {
      if(m_pipe) {
        m_pipe->add(t);
      }      
    }

  private:  
    virtual AbstractPipe* getPipe() override
    {
      return m_pipe.get();
    }

    friend void connect<T>(OutputPort<T>& output, InputPort<T>& input);

    unique_ptr<Pipe<T>> m_pipe;
  };
}