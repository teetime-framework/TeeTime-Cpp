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
#include <teetime/Md5Hash.h>
#include <vector>

namespace teetime
{
  Md5Hash md5hash(int i)
  {
    return Md5Hash::generate(&i, sizeof(i));
  }

  Md5Hash md5hash(float f)
  {
    return Md5Hash::generate(&f, sizeof(f));
  }

  Md5Hash md5hash(const char* s)
  {
    return Md5Hash::generate(s, strlen(s));
  }

  Md5Hash md5hash(const std::string& s)
  {
    return Md5Hash::generate(s);
  }

  Md5Hash md5hash(const std::vector<char>& bytes)
  {
    return Md5Hash::generate(bytes.data(), bytes.size());
  }
}
