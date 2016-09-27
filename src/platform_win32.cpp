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
#include <algorithm>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shellapi.h>
#include <Shlobj.h>
#include <tchar.h>
#include <strsafe.h>


static inline std::string fixpath(std::string s)
{
  std::replace(s.begin(), s.end(), '/', '\\');
  return s;
}

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
    auto winpath = fixpath(path);

    const DWORD d = GetFileAttributesA(winpath.c_str());
    return (d != INVALID_FILE_ATTRIBUTES) && !(d & FILE_ATTRIBUTE_DIRECTORY);
  }

  bool isDirectory(const char* path)
  {
    assert(path);
    auto winpath = fixpath(path);

    const DWORD d = GetFileAttributesA(winpath.c_str());
    return (d != INVALID_FILE_ATTRIBUTES) && (d & FILE_ATTRIBUTE_DIRECTORY);
  }

  bool removeFile(const char* path)
  {
    assert(path);
    auto winpath = fixpath(path);

    BOOL ret = DeleteFileA(winpath.c_str());
    return (ret == TRUE);
  }

  bool createDirectory(const char* path)
  {
    assert(path);
    auto winpath = fixpath(path);

    auto ret = SHCreateDirectoryEx(NULL, winpath.c_str(), NULL);
    return ret == ERROR_SUCCESS;
  }

  bool getCurrentWorkingDirectory(char* buffer, size_t buffersize)
  {
    const DWORD d = GetCurrentDirectoryA(static_cast<DWORD>(buffersize), buffer);

    if (d == 0 || d == sizeof(buffer))
      return false;

    for (DWORD i = 0; i < d; ++i) {
      if (buffer[i] == '\\')
        buffer[i] = '/';
    }

    return true;
  }



  static bool listDirectoryContent(const char* directory, std::vector<std::string>& entries, bool recursive, bool files, bool directories)
  {
    assert(directory);
    auto winpath = fixpath(directory);

    WIN32_FIND_DATA ffd;

    TCHAR szDir[MAX_PATH];
    StringCchCopy(szDir, MAX_PATH, winpath.c_str());
    StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

    HANDLE hFind = FindFirstFileA(szDir, &ffd);

    if (INVALID_HANDLE_VALUE == hFind)
    {
      TEETIME_DEBUG() << "Directory not found: " << directory;
      return false;
    }

    do
    {
      if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
        //ignore '.' and '..' entries
        if (strcmp(ffd.cFileName, ".") != 0 && strcmp(ffd.cFileName, "..") != 0)
        {
          if (directories)
          {
            entries.push_back(std::string(ffd.cFileName));
          }

          std::string d = std::string(directory) + "/" + std::string(ffd.cFileName);
          std::vector<std::string> subEntries;
          if (recursive)
          {
            if (!listDirectoryContent(d.c_str(), subEntries, recursive, files, directories))
              return false;

            for (const auto& e : subEntries)
            {
              entries.push_back(std::string(ffd.cFileName) + "/" + e);
            }
          }
        }
      }
      else
      {
        if (files) 
        {
          entries.push_back(std::string(ffd.cFileName));
        }
      }
    } while (FindNextFileA(hFind, &ffd) != 0);

    FindClose(hFind);
    return true;
  }

  bool listFiles(const char* directory, std::vector<std::string>& entries, bool recursive)
  {
    return listDirectoryContent(directory, entries, recursive, true, false);
  }

  bool listSubDirectories(const char* directory, std::vector<std::string>& entries, bool recursive)
  {
    return listDirectoryContent(directory, entries, recursive, false, true);
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