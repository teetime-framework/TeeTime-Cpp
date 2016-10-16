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

#define NO_VOLATILE_POINTERS

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
#include <map>
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
#include "../fastflow/buffer.hpp"
TEETIME_WARNING_POP



enum class QueueType {
  None,
  Folly,
  FollyOpt,
  Boost,
  TeeTimeValue,
  TeeTimePointer,
  FastFlow,
  FastFlowNoVolatile,
  NUM
};

QueueType getQueueType(int argc, char** argv) {

  auto printTypes = []() {
    std::cout << std::endl;
    std::cout << static_cast<int>(QueueType::Folly) << ": Folly" << std::endl;
    std::cout << static_cast<int>(QueueType::FollyOpt) << ": FollyOpt" << std::endl;
    std::cout << static_cast<int>(QueueType::Boost) << ": Boost" << std::endl;
    std::cout << static_cast<int>(QueueType::TeeTimeValue) << ": TeeTimeValue" << std::endl;
    std::cout << static_cast<int>(QueueType::TeeTimePointer) << ": TeeTimePointer" << std::endl;
    std::cout << static_cast<int>(QueueType::FastFlow) << ": FastFlow" << std::endl;
    std::cout << static_cast<int>(QueueType::FastFlowNoVolatile) << ": FastFlowNoVolatile" << std::endl;
  };

  if (argc < 2) {
    printTypes();
    return QueueType::None;
  }

  int i = ::atoi(argv[1]);
  if (i > 0 && i < static_cast<int>(QueueType::NUM)) {
    return static_cast<QueueType>(i);
  }

  printTypes();
  return QueueType::None;
}

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

template<typename T>
class FastFlowNoVolatileQueue
{
  static_assert(std::is_pointer<T>::value, "T must be a pointer type");
public:
  explicit FastFlowNoVolatileQueue(size_t capacity)
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
  ff::SWSR_Ptr_Buffer_NoVolatile m_buffer;
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
    return boost::lockfree::spsc_queue<T>::pop(t);
  }
};

#endif

using namespace teetime;






template<template<typename> class TQueue>
uint64 benchmark_pointer2(size_t numValues, size_t capacity)
{
  TQueue<void*> pipe(capacity);
  std::vector<void*> source;
  source.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    source.push_back((void*)(i + 1));
  }

  size_t dest = 0;

  auto produce = [&]() {
    const size_t local_num = numValues;

    for (size_t i = 0; i < local_num; ++i)
    {
      while (true)
      {
        if (pipe.write(std::move(source[i])))
          break;
        else
          std::this_thread::yield();
      }
    }
  };

  auto consume = [&]() {
    const size_t local_num = numValues;
    for (size_t i = 0; i < local_num; ++i)
    {
      void* tmp;
      while (true)
      {

        if (pipe.read(tmp))
        {
          dest += (size_t)tmp;
          break;
        }
        else
        {
          std::this_thread::yield();
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

  return (end - start);
}




template<template<typename> class TQueue>
uint64 benchmark_map(size_t numValues, size_t capacity)
{
  using intmap = std::map<int, int>;

  TQueue<intmap> pipe(capacity);
  std::vector<intmap> source;
  std::vector<intmap> dest;
  dest.reserve(numValues);
  source.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    intmap map;
    map[(int)i] = i * 2;

    source.push_back(std::move(map));
  }

  auto produce = [&]() {
    const size_t local_num = numValues;

    for (size_t i = 0; i < local_num; ++i)
    {
      while (true)
      {
        if (pipe.write(std::move(source[i])))
          break;
        else
          std::this_thread::yield();
      }
    }
  };

  auto consume = [&]() {
    const size_t local_num = numValues;
    for (size_t i = 0; i < local_num; ++i)
    {
      intmap tmp;
      while (true)
      {

        if (pipe.read(tmp))
        {
          dest.push_back(std::move(tmp));
          break;
        }
        else
        {
          std::this_thread::yield();
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

  return (end - start);
}




void run(size_t iterations, size_t numValues, size_t capacity, const char* name, uint64(*benchmark_f)(size_t, size_t))
{
  uint64 sum = 0;

  for (int i = 0; i < iterations; ++i)
  {
    //auto ns = benchmark<folly::ProducerConsumerQueue>();    
    auto ns = benchmark_f(numValues, capacity);
    TEETIME_INFO() << "    run " << i << ": " << (ns * 0.001) << "ms";
    //std::cout << i << ": " << ns << std::endl;
    sum += ns;
  }

  std::cout << name << ": " << (double)sum / iterations * 0.001 << "ms" << std::endl;
}


template<template<typename> class TQueue>
uint64 foo()
{
  TQueue<void*> queue(4096);

  auto start = teetime::platform::microSeconds();



  for (int i = 1; i < 1000; ++i) {
    int valueProduced = 0;
    int valueConsumed = 0;

    while (true) {
      if (!queue.write((void*)i))
        break;

      valueProduced++;
    }

    while (true) {
      void* p = nullptr;
      if (!queue.read(p))
        break;

      if (p == (void*)i)
        valueConsumed++;
    }

    if (valueProduced != valueConsumed)
    {
      std::cout << "ERROR" << std::endl;
    }
  }

  auto end = teetime::platform::microSeconds();
  return end - start;
}
