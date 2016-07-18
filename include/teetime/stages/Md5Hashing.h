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

#ifndef MD5_FUNCTIONAL_STAGE

#include <teetime/stages/AbstractConsumerStage.h>
#include <string>

namespace teetime
{
  class Md5Hash;

  class Md5Hashing final : public AbstractConsumerStage<std::string>
  {
  public:
    explicit Md5Hashing(const char* debugName = "Md5Hashing");
    OutputPort<Md5Hash>& getOutputPort();

  private:
    virtual void execute(const std::string& value) override;

    OutputPort<Md5Hash>* m_outputPort;
  };
}

#else
#include <teetime/stages/FunctionStage.h>
#include <teetime/Md5Hash.h>

namespace teetime
{
  inline Md5Hash generateMd5hashFromString(const std::string& s)
  {
    return Md5Hash::generate(s);
  }

  using Md5Hashing = FunctionStage<const std::string&, Md5Hash, generateMd5hashFromString>;
}

#endif