#include "common.h"




template<template<typename> class TQueue>
uint64 benchmark_matrix(size_t numValues, size_t capacity)
{
  struct Mat4
  {
    Mat4()
    {
      memset(m, 0, sizeof(m));
      m[0] = 1;
      m[5] = 1;
      m[10] = 1;
      m[15] = 1;
    }

    double m[16];
  };

  TQueue<Mat4> queue(capacity);
  std::vector<Mat4> dest;
  dest.reserve(numValues);

  auto produce = [&]() {
    const size_t local_num = numValues;
    Mat4 m;
    for (size_t i = 0; i < local_num; ++i)
    {
      while (!queue.write(m))
      {
        std::this_thread::yield();
      }
    }
  };

  auto consume = [&]() {
    const size_t local_num = numValues;

    Mat4 tmp;
    for (size_t i = 0; i < local_num; ++i)
    {      
      while (!queue.read(tmp))
      {
        std::this_thread::yield();
      }
      dest.push_back(std::move(tmp));
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


int main(int argc, char** argv)
{
  setLogCallback(::teetime::simpleLogging);
  setLogLevel(getLogLevelFromArgs(argc, argv));
  size_t iterations = 3;
  size_t capacity = 1024;

  int numValues = 10000000;

  Params params = getQueueType(argc, argv, "queue_matrix");
  switch (params.type) {
  case QueueType::Folly:
    run(iterations, numValues, capacity, "Folly", benchmark_matrix<folly::ProducerConsumerQueue>, params.filename);
    break;
  case QueueType::FollyOpt:
    run(iterations, numValues, capacity, "FollyOpt", benchmark_matrix<folly::AlignedProducerConsumerQueue>, params.filename);
    break;
  case QueueType::Boost:
#ifdef TEETIME_HAS_BOOST
    run(iterations, numValues, capacity, "boost::spsc_queue", benchmark_matrix<BoostSpscQueue>, params.filename);
    break;
#else
    std::cout << "boost not available" << std::endl;
    return EXIT_FAILURE;
#endif    
  case QueueType::TeeTimePointer:
    std::cout << "values not supported" << std::endl;
    return EXIT_FAILURE;
  case QueueType::TeeTimeValue:
    run(iterations, numValues, capacity, "SpscValueQueue", benchmark_matrix<SpscValueQueue>, params.filename);
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
