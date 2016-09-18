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

#include <teetime/BufferedFile.h>
#include <fstream>

using namespace teetime;

bool BufferedFile::load(const char* path)
{
  assert(path);

  std::ifstream file(path, std::ios::binary | std::ios::in);

  file.seekg(0, file.end);
  std::streamsize size = file.tellg();

  if (file.fail())
    return false;

  assert(size >= 0);
  if (static_cast<size_t>(size) > std::numeric_limits<size_t>::max())
  {
    TEETIME_ERROR() << "file too big: " << path;
    return false;
  }

  file.seekg(0, std::ios::beg);

  m_path = path;
  m_bytes.resize(static_cast<size_t>(size));

  //don't try to read if size is 0 anyway (because buffer.bytes.data() may return null in this case)
  if (size == 0 || file.read((char*)m_bytes.data(), size))
  {
    return true;
  }

  return false;
}