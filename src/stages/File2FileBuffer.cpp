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

#include <teetime/stages/File2FileBuffer.h>
#include <teetime/ports/OutputPort.h>
#include <teetime/FileBuffer.h>
#include <teetime/File.h>

#include <fstream>
#include <limits>

using namespace teetime;

File2FileBuffer::File2FileBuffer(const char* debugName)
  : AbstractFilterStage<File, FileBuffer>(debugName)
{
}

void File2FileBuffer::execute(File&& value)
{
  std::ifstream file(value.path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();

  assert(size >= 0);
  if (static_cast<size_t>(size) > std::numeric_limits<size_t>::max())
  {
    TEETIME_ERROR() << "file too big: " << value.path;
    return;
  }

  file.seekg(0, std::ios::beg);

  FileBuffer buffer;
  buffer.path = value.path;
  buffer.bytes.resize(static_cast<size_t>(size));

  //don't try to read if size is 0 anyway (because buffer.bytes.data() may return null in this case)
	if (size == 0 || file.read((char*)buffer.bytes.data(), size))
	{
		getOutputPort().send(std::move(buffer));
	}
	else
	{
		TEETIME_DEBUG() << "failed to read file: " << value.path;
	}

}