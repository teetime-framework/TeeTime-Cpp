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
#include <teetime/stages/AbstractProducerStage.h>

namespace teetime
{
  class RandomIntProducer final : public AbstractProducerStage<int>
  {
  public:
    explicit RandomIntProducer(int min, int max, unsigned num, const char* debugName = "RandomIntProducer");

  private:
    virtual void execute() override;

    int m_min;
    int m_max;
    unsigned m_num;
  };
}
