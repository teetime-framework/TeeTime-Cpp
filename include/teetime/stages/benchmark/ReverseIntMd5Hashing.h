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
#include "../AbstractConsumerStage.h"
#include "../../Md5Hash.h"
#include <climits>

namespace teetime
{
  /**
   * @brief [brief description]
   * @details [long description]
   * @return [description]
   */
  class ReverseIntMd5Hashing : public AbstractConsumerStage<Md5Hash>
  {
  public:
    ReverseIntMd5Hashing()
     : AbstractConsumerStage<Md5Hash>("ReverseIntMd5Hashing")
    {
      m_outputPort = AbstractStage::addNewOutputPort<int>();
    }

    OutputPort<int>& getOutputPort()
    {
      assert(m_outputPort);
      return *m_outputPort;
    }

    virtual void execute(Md5Hash&& hash) override
    {
      int reversed = -1;
      for(int i=0; i<INT_MAX; ++i)
      {
        if(hash == Md5Hash::generate(&i, sizeof(i)))
        {
          reversed = i;
          break;
        }
      }

      assert(m_outputPort);
      m_outputPort->send(std::move(reversed));
    }

  private:
    OutputPort<int>* m_outputPort;
  };
}



