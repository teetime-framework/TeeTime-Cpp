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

namespace teetime
{ 
  template<typename T>
  class BlockingRoundRobinDistribution
  {
  public:
    BlockingRoundRobinDistribution()
      : m_next(0)
    {}

    BlockingRoundRobinDistribution(const BlockingRoundRobinDistribution&) = default;
    ~BlockingRoundRobinDistribution() = default;
    BlockingRoundRobinDistribution& operator=(const BlockingRoundRobinDistribution&) = default;

    void operator()(const std::vector<unique_ptr<AbstractOutputPort>>& ports, T&& value) 
    {
      const size_t numOutputPorts = ports.size();
      assert(numOutputPorts > 0);

      const size_t index = (m_next == numOutputPorts) ? 0 : m_next;
      assert(index < numOutputPorts);

      auto abstractPort = ports[index].get();
      assert(abstractPort);

      auto typedPort = unsafe_dynamic_cast<OutputPort<T>>(abstractPort);
      assert(typedPort);

      m_next = index + 1;

      typedPort->send(std::move(value));
    }
    
  private:
    size_t m_next;
  };

  template<typename T>
  class CopyDistribution
  {
  public:
    CopyDistribution()
    {}

    CopyDistribution(const CopyDistribution&) = default;
    ~CopyDistribution() = default;
    CopyDistribution& operator=(const CopyDistribution&) = default;

    void operator()(const std::vector<unique_ptr<AbstractOutputPort>>& ports, T&& value)
    {
      assert(ports.size() > 0);

      const size_t numOutputPorts = ports.size();

      //skip very first port
      for (size_t i = 1; i < numOutputPorts; ++i)
      {
        assert(ports[i]);

        //don't std::move! we want a copy here
        send(ports[i], std::move(T(value)));
      }

      //save one copy, by moving the value to the first port (which we initially skipped)
      send(ports[0], std::move(value));
    }

  private:
    void send(const unique_ptr<AbstractOutputPort>& abstractPort, T&& value)
    {
      assert(abstractPort);

      auto typedPort = unsafe_dynamic_cast<OutputPort<T>>(abstractPort.get());
      assert(typedPort);

      typedPort->send(std::move(value));
    }
  };



  template<typename T, typename TDistributionPolicy = BlockingRoundRobinDistribution<T>>
  class DistributorStage final : public AbstractConsumerStage<T>
  {
  public:
    explicit DistributorStage(const char* debugName = "DistributorStage", TDistributionPolicy distribution = TDistributionPolicy())
    : AbstractConsumerStage<T>(debugName)
    , m_policy(distribution)
    {}

    OutputPort<T>& getNewOutputPort() 
    {
      OutputPort<T>* p = AbstractStage::addNewOutputPort<T>();
      return *p;
    }

  private:
    virtual void execute(T&& value) override
    {
      m_policy(AbstractStage::getOutputPorts(), std::move(value));
    }    

    TDistributionPolicy m_policy;
  };
}