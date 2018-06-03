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
#pragma once
#include <teetime/stages/AbstractConsumerStage.h>
#include <teetime/FileBuffer.h>

namespace teetime
{
  class Image;

  class ReadImage final : public AbstractConsumerStage<FileBuffer>
  {
  public:
    explicit ReadImage(const char* debugName = "ReadImage");
    OutputPort<Image>& getOutputPort();

  private:
    virtual void execute(FileBuffer&& buffer) override;

    OutputPort<Image>* m_outputPort;
  };
}