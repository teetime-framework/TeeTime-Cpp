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

#include <teetime/platform.h>
#include <sys/time.h>

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
    
  }
}
}
