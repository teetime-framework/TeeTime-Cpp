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

#if 1

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

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
#include <ff/allocator.hpp>

TEETIME_WARNING_POP

using namespace ff;
using teetime::Md5Hash;
using teetime::Params;

namespace
{
  struct task
  {
    explicit task(int i)
      : i(i)
    {}

    explicit task(const Md5Hash& hash)
      : hash(hash)
    {}

    int i;
    Md5Hash hash;
  };

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

  struct Producer : ff_node_t<char, task>
  {
    Producer(int num, int min, int max)
      : m_num(num)
      , m_min(min)
      , m_max(max)
    {
    }

    task* svc(char*)
    {
      if (m_min == m_max)
      {
        auto hash = Md5Hash::generate(&m_min, sizeof(m_min));

        for (int i = 0; i < m_num; ++i)
        { 
          ff_send_out(ff_new<task>(hash));
        }
      }
      else
      {
        std::mt19937                        generator(0);
        std::uniform_int_distribution<int>  distr(m_min, m_max);

        for (int i = 0; i < m_num; ++i)
        {
          int value = distr(generator);
          TEETIME_TRACE() << "produced: " << value;

          auto hash = Md5Hash::generate(&value, sizeof(value));
          TEETIME_TRACE() << "generated hash: " << hash.toHexString();

          ff_send_out(ff_new<task>(hash));
        }
      }

      return EOS;
    }

  private:
    int m_num;
    int m_min;
    int m_max;
  };


  struct HashCracker : ff_node_t<task, task>
  {
    task* svc(task* value)
    {
      for (int i = 0; i < INT_MAX; ++i) {
        if (Md5Hash::generate(&i, sizeof(i)) == value->hash) {
          value->i = i;
          break;
        }
      }

      return value;
    }
  };

  struct Sink : ff_node_t<task, char>
  {
    virtual char* svc(task* value) override
    {
      m_values.push_back(value->i);
      ff_delete(value);
      return GO_ON;
    }

    std::vector<int> m_values;
  };

}

void benchmark_fastflow_allocator(const Params& params, int threads)
{
  int min = params.getInt32("minvalue");
  int max = params.getInt32("maxvalue");
  int num = params.getInt32("num");

  std::vector<std::unique_ptr<ff_node>> W;
  for (size_t i = 0; i<threads; ++i)
    W.push_back(std::unique_ptr<ff_node_t<task, task> >(make_unique<HashCracker>()));

  Producer producer(num, min, max);
  Sink sink;

  ff_Farm<Md5Hash, char> farm(std::move(W), producer, sink);

  ff_Pipe<> pipe(farm);

  if (pipe.run_and_wait_end()<0)
    error("running pipe\n");
}

#endif