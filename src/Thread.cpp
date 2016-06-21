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
#include <teetime/Thread.h>
#include <teetime/logging.h>
using namespace teetime;

Thread::Thread(AbstractStage* stage)
  : m_stage(stage)
{
}

void Thread::start()
{
  if(!m_stage) {
    TEETIME_ERROR() << "No stage to execute";
    return;
  }

  m_thread = std::thread([&](){


    m_stage->executeStage();
  });
}

void Thread::join()
{
  m_thread.join();
}

std::thread::id Thread::currentId()
{
  return std::this_thread::get_id();
}

void Thread::currentName(char * buffer, int buffersize)
{
  if (buffer && buffersize > 0)
  {
    *buffer = '\0';
  }
}
