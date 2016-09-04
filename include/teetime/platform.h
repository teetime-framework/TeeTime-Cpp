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

namespace teetime
{
namespace platform
{
  uint64 microSeconds();
  bool isFile(const char* path);
  bool removeFile(const char* path);

  void yield();

  void setThreadAffinityMask(unsigned mask);

  void* aligned_malloc(size_t size, size_t align);
  void  aligned_free(void* p);

#ifndef TEETIME_CACHELINESIZE
#define TEETIME_CACHELINESIZE 64
#endif
  static const size_t CacheLineSize = TEETIME_CACHELINESIZE;
}
}