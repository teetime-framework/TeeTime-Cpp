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
#include <teetime/common.h>

namespace teetime
{ 
  class Md5Hash final
  {
  public:
    Md5Hash();
    Md5Hash(const Md5Hash&) = default;
    Md5Hash& operator=(const Md5Hash&) = default;

    bool operator==(const Md5Hash& other) const;
    bool operator!=(const Md5Hash& other) const;

    std::string toHexString() const;    

    static Md5Hash generate(const void* data, size_t dataSize);
    static Md5Hash generate(const std::string& s);

  private:
    uint8 value[16];
  };
}