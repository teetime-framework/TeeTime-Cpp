#include "common.h"


template<template<typename> class TQueue>
uint64 benchmark_pointers_yield(size_t numValues, size_t capacity)
{
  TQueue<size_t*> pipe(capacity);
  std::vector<size_t*> dest;
  dest.reserve(numValues);

  auto produce = [&]() {
    const size_t local_num = numValues;

    for (size_t i = 1; i < local_num; ++i)
    {
      while (true)
      {
        if (pipe.write(reinterpret_cast<size_t*>(i)))
        {
          break;
        }
        else
        {
          std::this_thread::yield();
        }
      }
    }
  };

  auto consume = [&]() {
    const size_t local_num = numValues;

    for (size_t i = 1; i < local_num; ++i)
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

  for (size_t i = 0; i < dest.size(); ++i)
  {
    size_t expected = i + 1;

    if (dest[i] != reinterpret_cast<size_t*>(expected)) {
      std::cout << "ERROR: " << expected << ":" << dest[i] << std::endl;
      break;
    }
  }

  return (end - start);
}



int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));
  size_t iterations = 3;
  size_t capacity = 1024;

  int numValues = 10000000;

  Params params = getQueueType(argc, argv, "queue_pointer");
  switch (params.type) {
  case QueueType::Folly:
    run(iterations, numValues, capacity, "Folly", benchmark_pointers_yield<folly::ProducerConsumerQueue>, params.filename);
    break;
  case QueueType::FollyOpt:
    run(iterations, numValues, capacity, "FollyOpt", benchmark_pointers_yield<folly::AlignedProducerConsumerQueue>, params.filename);
    break;
  case QueueType::Boost:
#ifdef TEETIME_HAS_BOOST
    run(iterations, numValues, capacity, "boost::spsc_queue", benchmark_pointers_yield<BoostSpscQueue>, params.filename);
    break;
#else
    std::cout << "boost not available" << std::endl;
    return EXIT_FAILURE;
#endif    
  case QueueType::TeeTimePointer:
    run(iterations, numValues, capacity, "SpscPointerQueue", benchmark_pointers_yield<SpscPointerQueue>, params.filename);
    break;
  case QueueType::TeeTimeValue:
    run(iterations, numValues, capacity, "SpscValueQueue", benchmark_pointers_yield<SpscValueQueue>, params.filename);
    break;
  case QueueType::FastFlow:
    run(iterations, numValues, capacity, "FastFlowQueue", benchmark_pointers_yield<FastFlowQueue>, params.filename);
    break;
  case QueueType::FastFlowNoVolatile:
    run(iterations, numValues, capacity, "FastFlowNoVolatileQueue", benchmark_pointers_yield<FastFlowNoVolatileQueue>, params.filename);
    break;
  default:
    std::cout << "unknown or unsupported queue type" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
