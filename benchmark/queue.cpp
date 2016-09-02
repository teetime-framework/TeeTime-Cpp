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

#include <teetime/common.h>
#include <teetime/platform.h>
#include <teetime/logging.h>
#include <teetime/pipes/ProducerConsumerQueue.h>
TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT
#include <teetime/pipes/AlignedProducerConsumerQueue.h>
TEETIME_WARNING_POP
#include <teetime/pipes/SpscQueue.h>
#include <iostream>
#include <vector>
#include <string>
#include <type_traits>

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
#include "fastflow/buffer.hpp"
TEETIME_WARNING_POP

template<typename T>
class FastFlowQueue
{
  static_assert(std::is_pointer<T>::value, "T must be a pointer type");
public:
  explicit FastFlowQueue(size_t capacity)
    : m_buffer(static_cast<unsigned long>(capacity))
  {
    m_buffer.init();
  }

  bool write(T&& t)
  {
    return m_buffer.push(t);
  }

  bool write(const T& t)
  {
    return m_buffer.push(t);
  }

  bool read(T& t)
  {
    return m_buffer.pop(reinterpret_cast<void**>(&t));
  }

private:
  ff::SWSR_Ptr_Buffer m_buffer;
};


#ifdef TEETIME_HAS_BOOST
#include <boost/lockfree/spsc_queue.hpp>

template<typename T>
class BoostSpscQueue : public boost::lockfree::spsc_queue<T>
{
public:
  explicit BoostSpscQueue(size_t capacity)
    : boost::lockfree::spsc_queue<T>(capacity)
  {}

  bool write(T&& t)
  {
    return boost::lockfree::spsc_queue<T>::push(std::move(t));
  }

  bool write(const T& t)
  {
    return boost::lockfree::spsc_queue<T>::push(t);
  }

  bool read(T& t)
  {
    return boost::lockfree::spsc_queue<T>::pop(&t, 1) == 1;
  }
};

#endif

using namespace teetime;

template<template<typename> class TQueue>
uint64 benchmark_value(size_t numValues, size_t capacity)
{
  TQueue<std::string> pipe(capacity);
  std::vector<std::string> dest;
  dest.reserve(numValues);

  std::atomic<bool> started(false);
  std::atomic<int> ready(0);

  auto produce = [&](){ 
    platform::setThreadAffinityMask(1);
    const size_t local_num = numValues;

    ready.fetch_add(1);
    while(started.load() == false)
    {
    }

    for(size_t i=0; i<local_num; ++i)
    {
      std::string s(64, 't');
      while(true)
      {
        if (pipe.write(std::move(s)))
          break;
        else
          platform::yield();
      }
    }
  };

  auto consume = [&](){
    platform::setThreadAffinityMask(2);
    const size_t local_num = numValues;

    ready.fetch_add(1);
    while(started.load() == false)
    {
    }

    for(size_t i=0; i<local_num; ++i)
    {
      std::string val;
      while(true)
      {
        if(pipe.read(val))
        {
          dest.push_back(std::move(val));
          break;
        }
        else
        {
          platform::yield();
        }
      }
    }
  }; 


  std::thread consumer(consume);
  std::thread producer(produce);

  while(ready.load() != 2)
  {
  }
  
  auto start = teetime::platform::microSeconds();
  started.store(true);

  producer.join();
  consumer.join();
  auto end = teetime::platform::microSeconds();

  std::string s(64, 't');
  for(size_t i=0; i<dest.size(); ++i)
  {
    if(dest[i] != s) {
      std::cout << "ERROR: " << i << ":" << dest[i] << std::endl;
      break;
    }
  }  

  return (end - start);
}

template<template<typename> class TQueue>
uint64 benchmark2(size_t numValues, size_t capacity)
{
  TQueue<size_t*> pipe(capacity);
  std::vector<size_t*> dest;
  dest.reserve(numValues);

  auto produce = [&]() {
    platform::setThreadAffinityMask(1);
    const size_t local_num = numValues;

    for (size_t i = 1; i<local_num; ++i)
    { 
      while (true)
      {
        if (pipe.write(reinterpret_cast<size_t*>(i)))
        {
          break;
        }
        else
        {
          platform::yield();
        }
      }
    }
  };

  auto consume = [&]() {
    platform::setThreadAffinityMask(2);
    const size_t local_num = numValues;

    for (size_t i = 1; i<local_num; ++i)
    {
      size_t* val;
      while (true)
      {
        if (pipe.read(val))
        {
          dest.push_back(val);
          break;
        }
        else
        {
          platform::yield();
        }
      }
    }
  };

  auto start = teetime::platform::microSeconds();
  std::thread consumer(consume);
  std::thread producer(produce);
  producer.join();
  consumer.join();
  auto end = teetime::platform::microSeconds();

  bool hasError = false;

  for (size_t i = 0; i<dest.size(); ++i)
  {
    size_t expected = i + 1;

    if (dest[i] != reinterpret_cast<size_t*>(expected)) {
      hasError = true;
      std::cout << "ERROR: " << expected << ":" << dest[i] << std::endl;
      break;
    }
  }

  return (end - start);
}



void run(size_t iterations, size_t numValues, size_t capacity, const char* name, uint64(*benchmark_f)(size_t, size_t))
{
  uint64 sum = 0;

  for (int i = 0; i < iterations; ++i)
  {
    //auto ns = benchmark<folly::ProducerConsumerQueue>();
    TEETIME_INFO() << "starting run " << i << "...";
    auto ns = benchmark_f(numValues, capacity);
    TEETIME_INFO() << "    run " << i << " done.";
    //std::cout << i << ": " << ns << std::endl;
    sum += ns;
  }

  std::cout << name << ": " << (double)sum / iterations * 0.001 << "ms" << std::endl;
}

int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));
  size_t iterations = 10;  
  size_t capacity = 1024;

  std::cout << "value based (std::string):" << std::endl;
  size_t numValues = 10000000L;
  run(iterations, numValues, capacity, "SpscValueQueue", benchmark_value<SpscValueQueue>);
  run(iterations, numValues, capacity, "Folly", benchmark_value<folly::ProducerConsumerQueue>);
  run(iterations, numValues, capacity, "Folly (cache optimized)", benchmark_value<folly::AlignedProducerConsumerQueue>);
#ifdef TEETIME_HAS_BOOST
  run(iterations, numValues, capacity, "boost::spsc_queue", benchmark_value<BoostSpscQueue>);
#endif

  std::cout << "pointer based (size_t*):" << std::endl;
  numValues = 100000000L;
  run(iterations, numValues, capacity, "SpscValueQueue", benchmark2<SpscValueQueue>);
  run(iterations, numValues, capacity, "SpscPointerQueue", benchmark2<SpscPointerQueue>);
  run(iterations, numValues, capacity, "Folly", benchmark2<folly::ProducerConsumerQueue>);
  run(iterations, numValues, capacity, "Folly (cache optimized)", benchmark2<folly::AlignedProducerConsumerQueue>);
  run(iterations, numValues, capacity, "FastFlowQueue", benchmark2<FastFlowQueue>);
#ifdef TEETIME_HAS_BOOST
  run(iterations, numValues, capacity, "boost::spsc_queue", benchmark2<BoostSpscQueue>);
#endif
}
