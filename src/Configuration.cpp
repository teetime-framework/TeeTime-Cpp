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
#include <teetime/Configuration.h>
#include <teetime/Runnable.h>

using namespace teetime;

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

void Configuration::executeBlocking()
{
  std::vector<std::thread> threads;

  for(const auto& s : m_stages)
  {
    if(auto runnable = s->getRunnable())
    {
      std::thread t([=](){
        runnable->run();
      });

      threads.push_back(std::move(t));
    }
  }      

  for(auto& t : threads)
  {
    t.join();
  }      
}
