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

#ifdef _WIN32

#include <teetime/platform.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace teetime
{
namespace platform
{
  uint64 microSeconds()
  {
    static LARGE_INTEGER frequency;
    static BOOL b = QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (1000000LL * now.QuadPart) / frequency.QuadPart;
  }

  bool isFile(const char* path)
  {
    assert(path);
    const DWORD d = GetFileAttributesA(path);
    return (d != INVALID_FILE_ATTRIBUTES) && !(d & FILE_ATTRIBUTE_DIRECTORY);
  }

  bool removeFile(const char* path)
  {
    BOOL ret = DeleteFileA(path);
    return (ret == TRUE);
  }

  void yield()
  {
    SwitchToThread();
  }

  void setThreadAffinityMask(unsigned mask)
  {
    SetThreadAffinityMask(GetCurrentThread(), mask);
  }

  void* aligned_malloc(size_t size, size_t align)
  {
    return _mm_malloc(size, align);
  }

  void  aligned_free(void* p)
  {
    _mm_free(p);
  }
}
}

#endif