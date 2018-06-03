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
#include <teetime/stages/AbstractFilterStage.h>

namespace teetime
{
  class Md5Hash;

  Md5Hash md5hash(int i);
  Md5Hash md5hash(float f);
  Md5Hash md5hash(const char* s);
  Md5Hash md5hash(const std::string& s);
  Md5Hash md5hash(const std::vector<char>& bytes);

  template<typename T>
  class Md5Hashing final : public AbstractFilterStage<T, Md5Hash>
  {
  public:
    explicit Md5Hashing(const char* debugName = "Md5Hashing")
      : AbstractFilterStage<T, Md5Hash>(debugName)
    {}

  private:
    virtual void execute(T&& value) override
    {
      AbstractFilterStage<T, Md5Hash>::getOutputPort().send(md5hash(value));
    }
  };
}