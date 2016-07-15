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
#pragma once
#include <teetime/stages/ReadImage.h>
#include <teetime/ports/OutputPort.h>
#include <teetime/Image.h>

namespace teetime
{
  ReadImage::ReadImage(const char* debugName)
    : AbstractConsumerStage(debugName)
    , m_outputPort(nullptr)
  {
    m_outputPort = AbstractConsumerStage::addNewOutputPort<Image>();
  }

  OutputPort<Image>& ReadImage::getOutputPort()
  {
    return *m_outputPort;
  }

  void ReadImage::execute(const FileBuffer& buffer)
  {
    Image image;
    if (image.loadFromMemory(buffer.bytes.data(), buffer.bytes.size(), buffer.path.c_str()))
    {
      m_outputPort->send(image);
    }
  }
}