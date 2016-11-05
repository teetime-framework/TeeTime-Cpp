/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <teetime/stages/AbstractConsumerStage.h>
#include <thread>

namespace teetime
{ 
  /**
   * Delay the pipeline execution by sleeping for a certain number of milliseconds.
   * @tparam T type of data element to delay.
   */
  template<typename T>
  class DelayStage final : public AbstractConsumerStage<T>
  {
  public:
    explicit DelayStage(uint32 milliseconds, const char* debugName = "DelayStage")
    : AbstractConsumerStage<T>(debugName)    
    , m_outputPort(nullptr)
    , m_milliseconds(milliseconds)
    {
      m_outputPort = AbstractStage::addNewOutputPort<T>();
    }

    OutputPort<T>& getOutputPort() 
    {
      assert(m_outputPort);
      return *m_outputPort;
    }

  private:
    virtual void execute(T&& value) override
    {
      assert(m_outputPort);
      TEETIME_INFO() << this->debugName() << ": delaying " << value;
      std::this_thread::sleep_for(std::chrono::milliseconds(m_milliseconds));
      
      m_outputPort->send(std::move(value));
      TEETIME_INFO() << this->debugName() << ": delaying done";
    }    

    OutputPort<T>* m_outputPort;
    uint32 m_milliseconds;
  };
}