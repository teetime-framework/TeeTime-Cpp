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
#include <teetime/Configuration.h>
#include <teetime/Runnable.h>
#include <teetime/platform.h>
#include <teetime/ports/InputPort.h>
#include <teetime/ports/OutputPort.h>

using namespace teetime;

Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

void Configuration::createConnections()
{
  for (auto conn : m_connections)
  {
    auto settings = m_stageSettings[conn.in->owner()];
    (*conn.connectCallback)(conn.out, conn.in, conn.capacity, settings.isActive);
  }
}

void Configuration::declareStageActive(shared_ptr<AbstractStage> stage, unsigned cpus)
{
  m_stages.insert(stage);
  auto& s = m_stageSettings[stage.get()];
  s.cpuAffinity = cpus;
  s.isActive = true;
}

void Configuration::declareStageNonActive(shared_ptr<AbstractStage> stage)
{
  m_stages.insert(stage);
  auto& s = m_stageSettings[stage.get()];
  s.isActive = false;
}

bool Configuration::isPortConnected(const AbstractInputPort& port) const
{
  for (const auto& c : m_connections)
  {
    if (c.in == &port)
      return true;
  }

  return false;
}

bool Configuration::isPortConnected(const AbstractOutputPort& port) const
{
  for (const auto& c : m_connections)
  {
    if (c.out == &port)
      return true;
  }

  return false;
}

void Configuration::executeBlocking()
{
  createConnections();

  std::vector<std::thread> threads;

  for (const auto& s : m_stageSettings)
  {
    auto stage = s.first;
    auto settings = s.second;
    assert(stage);

    if (settings.isActive)
    {
      TEETIME_DEBUG() << "stage '" << stage->debugName() << "' is active";
      std::thread t([=]() {
        auto runnable = stage->createRunnable();
        assert(runnable);

        if (settings.cpuAffinity > 0)
        {
          ::teetime::platform::setThreadAffinityMask(settings.cpuAffinity);
        }

        TEETIME_INFO() << "thread created and initialized for stage " << stage->debugName();
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
