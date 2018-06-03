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
#include <teetime/common.h>
#include <vector>
#include <string>

namespace teetime
{
  /**
   * File buffer.
   */
  class BufferedFile
  {
  public:
    BufferedFile() = default;
    BufferedFile(const BufferedFile&) = default;
    ~BufferedFile() = default;
    BufferedFile& operator=(const BufferedFile&) = default;

    BufferedFile(BufferedFile&& f)
      : m_path(std::move(f.m_path))
      , m_bytes(std::move(f.m_bytes))
    {
    }

    BufferedFile& operator=(BufferedFile&& f)
    {
      m_path = std::move(f.m_path);
      m_bytes = std::move(f.m_bytes);
      return *this;
    }

    const char* path() const {
      return m_path.c_str();
    }

    size_t size() const {
      return m_bytes.size();
    }

    const uint8* data() const {
      return m_bytes.data();
    }

    bool load(const char* path);

    bool load(const std::string& path)
    {
      return load(path.c_str());
    }

  private:
    std::string m_path;
    std::vector<uint8> m_bytes;
  };
}