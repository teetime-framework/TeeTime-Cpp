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

#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <thread>
#include <random> 

#include <teetime/Md5Hash.h>
#include <teetime/logging.h>
#include "Benchmark.h"

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_UNREFERENCED_PARAMETER
TEETIME_WARNING_DISABLE_EMPTY_BODY
TEETIME_WARNING_DISABLE_MISSING_FIELD_INIT
TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT
TEETIME_WARNING_DISABLE_MAY_NOT_ALIGNED
TEETIME_WARNING_DISABLE_SIGNED_UNSIGNED_MISMATCH
TEETIME_WARNING_DISABLE_HIDDEN
TEETIME_WARNING_DISABLE_LOSSY_CONVERSION
TEETIME_WARNING_DISABLE_UNSAFE_USE_OF_BOOL
TEETIME_WARNING_DISABLE_UNREACHABLE

#define HAS_CXX11_VARIADIC_TEMPLATES
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

TEETIME_WARNING_POP

using namespace ff;
using teetime::Md5Hash;
using teetime::Params;

int writeAndReadFile(const char* fileprefix, int fileNum, const std::vector<char>& writeBuffer, std::vector<char>& readBuffer, int size);

namespace
{


  struct Producer : ff_node_t<char, int>
  {
    Producer(int num, int min, int max)
      : m_num(num)
      , m_min(min)
      , m_max(max)
    {
    }

    int* svc(char*)
    {
      if (m_min == m_max)
      {
        for (int i = 0; i < m_num; ++i)
        {
          ff_send_out(new int(m_min));
        }
      }
      else
      {
        std::mt19937                        generator(0);
        std::uniform_int_distribution<int>  distr(m_min, m_max);

        for (int i = 0; i < m_num; ++i)
        {
          int value = distr(generator);
         
          ff_send_out(new int(value));
        }
      }

      return EOS;
    }

  private:
    int m_num;
    int m_min;
    int m_max;
  };

  struct WriterReader : ff_node_t<int, int>
  {
    explicit WriterReader(const char* prefix)
      : m_prefix(prefix)
      , m_counter(0)
    {
      m_writeBuffer.resize(1024 * 1024 * 100, 'X');
      m_readBuffer.resize(1024 * 1024 * 100, '\0');
    }

    int* svc(int* value)
    {
      int ret = writeAndReadFile(m_prefix.c_str(), m_counter++, m_writeBuffer, m_readBuffer, *value);
      
      delete value;
      return new int(ret);
    }

  private:
    std::string m_prefix;
    int m_counter;

    std::vector<char> m_writeBuffer;
    std::vector<char> m_readBuffer;
  };

  struct Sink : ff_node_t<int, char>
  {
    virtual char* svc(int* value) override
    {
      m_values.push_back(*value);
      delete value;
      return GO_ON;
    }

    std::vector<int> m_values;
  };

}

void io_fastflow(const Params& params, int threads)
{
  std::vector<std::unique_ptr<ff_node>> W;
  for (size_t i = 0; i < threads; ++i)
  {
    char prefix[256];
    sprintf(prefix, "writer%d_", static_cast<int>(i));

    W.push_back(std::unique_ptr<ff_node_t<int, int> >(make_unique<WriterReader>(prefix)));
  }

  Producer producer(params.getInt32("num"), params.getInt32("minvalue"), params.getInt32("maxvalue"));
  Sink sink;

  ff_Farm<Md5Hash, char> farm(std::move(W), producer, sink);

  ff_Pipe<> pipe(farm);

  if (pipe.run_and_wait_end() < 0)
    error("running pipe\n");
}
