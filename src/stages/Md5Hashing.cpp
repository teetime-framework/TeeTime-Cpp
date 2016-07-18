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

#include <teetime/stages/Md5Hashing.h>

#ifndef MD5_FUNCTIONAL_STAGE
 
#include <teetime/ports/OutputPort.h>
#include <teetime/Md5Hash.h>
#include <memory.h>

using namespace teetime;

Md5Hashing::Md5Hashing(const char* debugName)
  : AbstractConsumerStage(debugName)
{
  m_outputPort = AbstractConsumerStage::addNewOutputPort<Md5Hash>();
}

OutputPort<Md5Hash>& Md5Hashing::getOutputPort()
{
  assert(m_outputPort);
  return *m_outputPort;
}

void Md5Hashing::execute(std::string&& value)
{
  assert(m_outputPort);
  m_outputPort->send(Md5Hash::generate(value));
}

#endif