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

#ifdef __linux__

#include <teetime/platform.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>


namespace teetime
{
namespace platform
{
  static bool listDirectoryContent(const char* path, std::vector<std::string>& entries, bool recursive, bool files, bool dirs)
  {
    assert(path);

    DIR* dp;
    struct dirent* ep;

    dp = opendir(path);
    if (dp)
    {
      while ((ep = readdir(dp)))
      {
        if (std::strcmp(".", ep->d_name) == 0 || std::strcmp("..", ep->d_name) == 0)
          continue;

        if (ep->d_type == DT_REG)
        {
          if(files)
            entries.push_back(ep->d_name);
        }
        else if (ep->d_type == DT_DIR)
        {
          if(dirs)
            entries.push_back(ep->d_name);

          if (recursive)
          {
            char buffer[256];
            sprintf(buffer, "%s/%s", path, ep->d_name);
            std::vector<std::string> subentries;
            if (!listDirectoryContent(buffer, subentries, recursive, files, dirs))
              return false;

            for (const auto& e : subentries)
            {
              entries.push_back(std::string(ep->d_name) + "/" + e);
            }
          }
        }
      }

      (void)closedir(dp);
      return true;
    }

    return false;
  }


  uint64 microSeconds()
  {
    static unsigned long sys_timeBase = 0;

    struct timeval tp;
    gettimeofday(&tp, NULL);

    if (!sys_timeBase) {
      sys_timeBase = tp.tv_sec;
      return tp.tv_usec;
    }

    return uint64(tp.tv_sec - sys_timeBase) * 1000000 + tp.tv_usec;
  }

  bool getCurrentWorkingDirectory(char* buffer, size_t buffersize)
  {
    char* path = getcwd(buffer, buffersize);

    return (path != nullptr);
  }

  bool isFile(const char* path)
  {
    assert(path);
    struct stat buf;

    if (stat(path, &buf) != -1)
      return (bool)S_ISREG(buf.st_mode);

    return false;
  }

  bool isDirectory(const char* path)
  {
    assert(path);
    struct stat buf;

    if (stat(path, &buf) != -1)
      return (bool)S_ISDIR(buf.st_mode);

    return false;
  }

  bool removeFile(const char* path)
  {
    return remove(path) == 0;
  }

  bool createDirectory(const char* path)
  {
    assert(path);
    static const mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH;

    return (mkdir(path, mode) == 0);
  }

  bool listFiles(const char* directory, std::vector<std::string>& entries, bool recursive)
  {
    return listDirectoryContent(directory, entries, recursive, true, false);
  }

  bool listSubDirectories(const char* directory, std::vector<std::string>& entries, bool recursive)
  {
    return listDirectoryContent(directory, entries, recursive, false, true);
  }

  void setThreadAffinityMask(unsigned mask)
  {
    if (mask == 0)
      return;

    pthread_t thread = pthread_self();

    /* Set affinity mask to include CPUs 0 to 7 */

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    for (int i = 0; i < 32; ++i)
    {
      if (mask & (1 << i))
      {
        CPU_SET(i, &cpuset);
      }
    }

    if (CPU_COUNT(&cpuset) == 0)
    {
      TEETIME_WARN() << "thread affinity mask ignored, thread must be assigned to at least one CPU";
      return;
    }

    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0)
    {
      TEETIME_WARN() << "failed to set thread affinity";
    }
  }

  void* aligned_malloc(size_t size, size_t align)
  {
    void *result;

    if (posix_memalign(&result, align, size))
    {
      result = nullptr;
    }

    return result;
  }

  void  aligned_free(void* p)
  {
    free(p);
  }
}
}


#endif