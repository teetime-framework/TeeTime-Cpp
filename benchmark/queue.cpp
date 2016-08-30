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

#include "fastflow/buffer.hpp"

static const size_t numValues = 10000000L;
static const size_t capacity = 1024;

template<typename T>
class FastFlowQueue
{
public:
  explicit FastFlowQueue(size_t capacity)
    : m_buffer(capacity)
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
uint64 benchmark_value()
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
void runValue(int num, const char* name)
{
  uint64 sum = 0;

  for (int i = 0; i<num; ++i)
  {
    //auto ns = benchmark<folly::ProducerConsumerQueue>();
    auto ns = benchmark_value<TQueue>();
    //std::cout << i << ": " << ns << std::endl;
    sum += ns;
  }

  std::cout << "[value] " << name << ": " << (double)sum / num * 0.001 << "ms" << std::endl;
}



template<template<typename> class TQueue>
uint64 benchmark2()
{
  TQueue<size_t*> pipe(capacity);
  std::vector<size_t*> dest;
  dest.reserve(numValues);

  std::atomic<bool> started(false);
  std::atomic<int> ready(0);

  size_t* tmp = nullptr;
  if (pipe.read(tmp))
  {
    TEETIME_DEBUG() << "WTF?";
  }

  auto produce = [&]() {
    platform::setThreadAffinityMask(1);
    const size_t local_num = numValues;

    ready.fetch_add(1);
    while (started.load() == false)
    {
    }

    for (size_t i = 1; i<local_num; ++i)
    {
      TEETIME_TRACE() << "producing " << i << "/" << local_num;
      while (true)
      {
        if (pipe.write(reinterpret_cast<size_t*>(i)))
          break;
        else
          platform::yield();
      }
    }

    TEETIME_TRACE() << "producer done";
  };

  auto consume = [&]() {
    platform::setThreadAffinityMask(2);
    const size_t local_num = numValues;

    ready.fetch_add(1);
    while (started.load() == false)
    {
    }

    for (size_t i = 1; i<local_num; ++i)
    {
      
      size_t* val;
      while (true)
      {
        if (pipe.read(val))
        {
          TEETIME_TRACE() << "consuming " << i << "/" << local_num << " (" << reinterpret_cast<size_t>(val) << ")";
          dest.push_back(val);
          break;
        }
        else
        {
          platform::yield();
        }
      }
    }

    TEETIME_TRACE() << "consumer done";
  };


  std::thread consumer(consume);
  std::thread producer(produce);

  while (ready.load() != 2)
  {
  }

  auto start = teetime::platform::microSeconds();
  started.store(true);

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



template<template<typename> class TQueue>
void run(int num, const char* name)
{
  uint64 sum = 0;

  for (int i = 0; i<num; ++i)
  {
    //auto ns = benchmark<folly::ProducerConsumerQueue>();
    TEETIME_INFO() << "starting run " << i << "...";
    auto ns = benchmark2<TQueue>();
    TEETIME_INFO() << "    run " << i << " done.";
    //std::cout << i << ": " << ns << std::endl;
    sum += ns;
  }

  std::cout << "[pointers] " << name << ": " << (double)sum / num * 0.001 << "ms" << std::endl;
}

int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));
  int num = 10;


  runValue<SpscValueQueue>(num, "SpscValueQueue");
  runValue<folly::ProducerConsumerQueue>(num, "Folly");
  runValue<folly::AlignedProducerConsumerQueue>(num, "Folly (cache aligned)");
#ifdef TEETIME_HAS_BOOST
  runValue<BoostSpscQueue>(num, "boost::spsc_queue");
#endif

  run<SpscValueQueue>(num, "SpscValueQueue");
  run<SpscPointerQueue>(num, "SpscPointerQueue");
  run<FastFlowQueue>(num, "FastFlowQueue");
  run<folly::ProducerConsumerQueue>(num, "Folly");
  run<folly::AlignedProducerConsumerQueue>(num, "Folly (cache aligned)");
#ifdef TEETIME_HAS_BOOST
  run<BoostSpscQueue>(num, "boost::spsc_queue");
#endif
}
