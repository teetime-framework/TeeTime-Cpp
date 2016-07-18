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

#include <teetime/stages/FileExtensionSwitch.h>
#include <teetime/ports/OutputPort.h>

using namespace teetime;

FileExtensionSwitch::FileExtensionSwitch(const char* debugName)
  : AbstractConsumerStage(debugName)
  , m_defaultOutputPort(nullptr)
{
}

OutputPort<File>& FileExtensionSwitch::getOutputPort(const std::string& extension)
{
  auto& port = m_outputPorts[extension];

  if (!port) 
  {
    port = AbstractConsumerStage::addNewOutputPort<File>();
  }

  return *port;
}

OutputPort<File>& FileExtensionSwitch::getDefaultOutputPort()
{
  if (!m_defaultOutputPort)
  {
    m_defaultOutputPort = AbstractConsumerStage::addNewOutputPort<File>();
  }

  return *m_defaultOutputPort;
}

void FileExtensionSwitch::execute(File&& value)
{
  auto index = value.path.find_last_of('.');

  auto ext = (index != std::string::npos) ? value.path.substr(index + 1) : "";
  auto port = m_outputPorts[ext];

  if (port)
  {
    port->send(std::move(value));
  }
  else if (m_defaultOutputPort)
  {
    m_defaultOutputPort->send(std::move(value));
  }
}