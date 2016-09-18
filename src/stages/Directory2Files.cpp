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

#include <teetime/stages/Directory2Files.h>
#include <teetime/ports/OutputPort.h>
#include <teetime/File.h>
#include <teetime/platform.h>

#include <algorithm>

using namespace teetime;

Directory2Files::Directory2Files(const char* debugName)
  : AbstractConsumerStage<std::string>(debugName)
  , m_outputPort(nullptr)
{
  m_outputPort = AbstractStage::addNewOutputPort<File>();
}

OutputPort<File>& Directory2Files::getOutputPort()
{
  assert(m_outputPort);
  return *m_outputPort;
}

void Directory2Files::execute(std::string&& value)
{
  std::vector<std::string> files;
  platform::listFiles(value.c_str(), files, true);
  
  std::sort(files.begin(), files.end());

  for (const auto& f : files)
  {
    File file;
    file.path = value + "/" + f;

    getOutputPort().send(std::move(file));
  }
}