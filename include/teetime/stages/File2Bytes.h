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
#include <teetime/stages/AbstractConsumerStage.h>
#include <teetime/stages/Directory2Files.h>
#include <string>

namespace teetime
{
  using ByteArray = std::vector<unsigned char>;

  class File2Bytes final : public AbstractConsumerStage<File>
  {
  public:

    explicit File2Bytes(const char* debugName = "File2Bytes");
    OutputPort<ByteArray>& getOutputPort();

  private:
    virtual void execute(const File& value) override;

    OutputPort<ByteArray>* m_outputPort;
  };
}