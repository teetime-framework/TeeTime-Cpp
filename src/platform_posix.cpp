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

#ifdef __linux__

#include <teetime/platform.h>
#include <sys/time.h>
#include <pthread.h>

namespace teetime
{
namespace platform
{
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

  void yield()
  {
    pthread_yield();
  }

  void setThreadAffinityMask(unsigned mask)
  {
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