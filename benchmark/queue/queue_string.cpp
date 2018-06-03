#include "common.h"
#include <string>


const char* getTestString(int i) {
  static char buffer[256];
  sprintf(buffer, "Hello world! This is TeeTime, the Pipe & Filter architecture framework: %d", i);
  return &buffer[0];
}

template<template<typename> class TQueue>
uint64 benchmark(size_t numValues, size_t capacity)
{
  TQueue<std::string> pipe(capacity);
  std::vector<std::string> source;
  std::vector<std::string> dest;
  dest.reserve(numValues);
  source.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    source.push_back(std::string(getTestString((int)i)));
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
    std::string tmp;
    for (size_t i = 0; i < local_num; ++i)
    {
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

  for (size_t i = 0; i < dest.size(); ++i)
  {
    if (dest[i] != getTestString((int)i)) {
      std::cout << "ERROR: " << i << ":" << dest[i].size() << std::endl;
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

  Params params = getQueueType(argc, argv, "queue_string");
  switch (params.type) {
  case QueueType::Folly:
    run(iterations, numValues, capacity, "Folly", benchmark<folly::ProducerConsumerQueue>, params.filename);
    break;
  case QueueType::FollyOpt:
    run(iterations, numValues, capacity, "FollyOpt", benchmark<folly::AlignedProducerConsumerQueue>, params.filename);
    break;
  case QueueType::Boost:
#ifdef TEETIME_HAS_BOOST
    run(iterations, numValues, capacity, "boost::spsc_queue", benchmark<BoostSpscQueue>, params.filename);
    break;
#else
    std::cout << "boost not available" << std::endl;
    return EXIT_FAILURE;
#endif
  case QueueType::TeeTimePointer:
    std::cout << "values not supported" << std::endl;
    return EXIT_FAILURE;
  case QueueType::TeeTimeValue:
    run(iterations, numValues, capacity, "SpscValueQueue", benchmark<SpscValueQueue>, params.filename);
    break;
  case QueueType::FastFlow:
    std::cout << "values not supported" << std::endl;
    return EXIT_FAILURE;
  case QueueType::FastFlowNoVolatile:
    std::cout << "values not supported" << std::endl;
    return EXIT_FAILURE;
  default:
    std::cout << "unknown or unsupported queue type" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
