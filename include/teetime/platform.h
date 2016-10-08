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
#include "common.h"
#include <vector>
#include <string>

namespace teetime
{
namespace platform
{
  uint64 microSeconds();
  bool createDirectory(const char* path);
  bool isFile(const char* path);
  bool isDirectory(const char* path);
  bool removeFile(const char* path);
  bool listFiles(const char* directory, std::vector<std::string>& entries, bool recursive);
  bool listSubDirectories(const char* directory, std::vector<std::string>& entries, bool recursive);
  bool getCurrentWorkingDirectory(char* buffer, size_t buffersize);

  inline bool createDirectory(const std::string& path) { return createDirectory(path.c_str()); }
  inline bool isFile(const std::string& path) { return isFile(path.c_str()); }
  inline bool isDirectory(const std::string& path) { return isDirectory(path.c_str()); }
  inline bool removeFile(const std::string& path) { return removeFile(path.c_str());  }
  inline bool listFiles(const std::string& path, std::vector<std::string>& entries, bool recursive) {
    return listFiles(path.c_str(), entries, recursive);
  }
  inline bool listSubDirectories(const std::string& path, std::vector<std::string>& entries, bool recursive) {
    return listSubDirectories(path.c_str(), entries, recursive);
  }
  inline bool getCurrentWorkingDirectory(std::string& s) {
    char buffer[256];
    if (getCurrentWorkingDirectory(buffer, sizeof(buffer))) {
      s = buffer;
      return true;
    } 

    return false;
  }

  void setThreadAffinityMask(unsigned mask);

  void* aligned_malloc(size_t size, size_t align);
  void  aligned_free(void* p);

#ifndef TEETIME_CACHELINESIZE
#define TEETIME_CACHELINESIZE 64
#endif
  static const size_t CacheLineSize = TEETIME_CACHELINESIZE;
}
}