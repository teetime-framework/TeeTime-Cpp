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
#include <teetime/stages/RandomIntProducer.h>
#include <teetime/ports/OutputPort.h>
#include <teetime/Image.h>
#include <random>

namespace teetime
{
  RandomIntProducer::RandomIntProducer(int min, int max, unsigned num, const char* debugName)
    : AbstractProducerStage<int>(debugName)
    , m_min(min)
    , m_max(max)
    , m_num(num)
  {
  }

  void RandomIntProducer::execute()
  {
    //std::random_device                  rand_dev;
    std::mt19937                        generator(0); //TODO(johl): currently using 0 as seed (instead of rand_dev) for reproducable results. This should be adjustable.
    std::uniform_int_distribution<int>  distr(m_min, m_max);

    for (unsigned i = 0; i < m_num; ++i)
    {
      int value = distr(generator);
      TEETIME_TRACE() << "random value produced: " << value;
      AbstractProducerStage<int>::getOutputPort().send(std::move(value));
    }

    AbstractProducerStage<int>::terminate();
  }
}