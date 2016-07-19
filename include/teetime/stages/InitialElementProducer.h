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
#include <teetime/stages/AbstractProducerStage.h>
#include <teetime/ports/OutputPort.h>
#include <vector>

namespace teetime
{
  template<typename T>
  class InitialElementProducer final : public AbstractProducerStage<T>
  {
  public:
    explicit InitialElementProducer(const T& element, const char* debugName = "InitialElementProducer")
      : AbstractProducerStage<T>(debugName)
    {
      m_elements.push_back(element);
    }

    explicit InitialElementProducer(const std::vector<T>& elements, const char* debugName = "InitialElementProducer")
      : AbstractProducerStage<T>(debugName)
      , m_elements(elements)
    {
    }

  private:
    virtual void execute() override
    {
      for (auto& e : m_elements) 
      {
        AbstractProducerStage<T>::getOutputPort().send(std::move(e));
      }

      AbstractProducerStage<T>::terminate();
    }

    std::vector<T> m_elements;
  };
}
