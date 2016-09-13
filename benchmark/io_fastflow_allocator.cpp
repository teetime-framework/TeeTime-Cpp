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

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <ff/allocator.hpp>

TEETIME_WARNING_POP

using namespace ff;
using teetime::Md5Hash;

int writeAndReadFile(const char* fileprefix, int fileNum, const std::vector<char>& writeBuffer, std::vector<char>& readBuffer, int size);

namespace
{
  template<typename T, typename... TArgs>
  T* ff_new(TArgs&&... args)
  {
    void* p = ff_malloc(sizeof(T));
    new (p) T(std::forward<TArgs>(args)...);
    return static_cast<T*>(p);
  }

  template<typename T>
  void ff_delete(T* p)
  {
    if (p)
    {
      p->~T();
      ff_free(p);
    }
  }

  struct Data
  {
    int bytesToWrite;
    int bytesRead;
  };


  struct Producer : ff_node_t<char, Data>
  {
    Producer(int num, int min, int max)
      : m_num(num)
      , m_min(min)
      , m_max(max)
    {
    }

    Data* svc(char*)
    {
      if (m_min == m_max)
      {
        for (int i = 0; i < m_num; ++i)
        {
          auto data = ff_new<Data>();
          data->bytesToWrite = m_min;

          ff_send_out(data);
        }
      }
      else
      {
        std::mt19937                        generator(0);
        std::uniform_int_distribution<int>  distr(m_min, m_max);

        for (int i = 0; i < m_num; ++i)
        {
          auto data = ff_new<Data>();
          data->bytesToWrite = distr(generator);

          ff_send_out(data);
        }
      }

      return EOS;
    }

  private:
    int m_num;
    int m_min;
    int m_max;
  };

  struct WriterReader : ff_node_t<Data, Data>
  {
    explicit WriterReader(const char* prefix)
      : m_prefix(prefix)
      , m_counter(0)
    {
      m_writeBuffer.resize(1024 * 1024 * 100, 'X');
      m_readBuffer.resize(1024 * 1024 * 100, '\0');
    }

    Data* svc(Data* value) override
    {
      int ret = writeAndReadFile(m_prefix.c_str(), m_counter++, m_writeBuffer, m_readBuffer, value->bytesToWrite);
      value->bytesRead = ret;
      return value;
    }

  private:
    std::string m_prefix;
    int m_counter;

    std::vector<char>        m_writeBuffer;
    std::vector<char>        m_readBuffer;
  };

  struct Sink : ff_node_t<Data, char>
  {
    virtual char* svc(Data* value) override
    {
      m_values.push_back(value->bytesRead);
      ff_delete(value);
      return GO_ON;
    }

    std::vector<int> m_values;
  };

}

void io_fastflow_allocator(int num, int min, int max, int threads)
{
  std::vector<std::unique_ptr<ff_node>> W;
  for (size_t i = 0; i < threads; ++i)
  {
    char prefix[256];
    sprintf(prefix, "writer%d_", static_cast<int>(i));

    W.push_back(std::unique_ptr<ff_node_t<Data, Data> >(make_unique<WriterReader>(prefix)));
  }

  Producer producer(num, min, max);
  Sink sink;

  ff_Farm<Md5Hash, char> farm(std::move(W), producer, sink);

  ff_Pipe<> pipe(farm);

  if (pipe.run_and_wait_end() < 0)
    error("running pipe\n");
}
