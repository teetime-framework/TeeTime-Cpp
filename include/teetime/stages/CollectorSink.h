/**
* Copyright (C) 2016 Johannes Ohlemacher (https://github.com/teetime-framework/TeeTime-Cpp)
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
#include <vector>

using namespace teetime;

namespace teetime {

  template<typename T>
  class CollectorSink : public AbstractConsumerStage<T>
  {
  public:
    explicit CollectorSink(size_t capacity = 16)
      : AbstractConsumerStage<T>("CollectorSink")
    {
      m_elements.reserve(capacity);
    }

    std::vector<T> getElements() const
    {
      return m_elements;
    }

    std::vector<T> takeElements()
    {
      return std::move(m_elements);
    }

  private:
    std::vector<T> m_elements;

    virtual void execute(T&& value) override
    {
      m_elements.push_back(std::move(value));
    }
  };

}