#include "common.h"
#include <memory>

class intlist
{
public:

  intlist()
    : m_size(0)
    , m_capacity(0)
  {
  }

  intlist(const intlist& l)
    : m_size(0)
    , m_capacity(0)
  {
    *this = l;
  }

  intlist(intlist&& l)
    : m_size(0)
    , m_capacity(0)
  {
    *this = std::move(l);
  }

  explicit intlist(size_t size, int value)
    : m_size(0)
    , m_capacity(0)
  {
    reserve(size);

    for(size_t i=0; i<size; ++i) {
      m_array[i] = value;
    }
  }

  int& operator[](size_t index) {
    return m_array[index];
  }

  int operator[](size_t index) const {
    return m_array[index];
  }

  bool operator==(const intlist& l)  const {
    if(m_size != l.m_size)
      return false;

    for(size_t i=0; i<m_size; ++i) {
      if(m_array[i] != l.m_array[i])
        return false;
    }

    return true;
  }

  bool operator!=(const intlist& l)  const {
    return !(*this == l);
  }

  intlist& operator=(intlist&& l) {
    std::swap(m_array, l.m_array);
    std::swap(m_size, l.m_size);
    std::swap(m_capacity, l.m_capacity);
    return *this;
  }

  intlist& operator=(const intlist& l) {
    reserve(l.m_size);
    for(size_t i=0; i<l.m_size; ++i) {
      m_array[i] = l.m_array[i];
    }
    return *this;
  }

  size_t size() const {
    return m_size;
  }

  void push_back(int i) {
    reserve(m_size + 1);
    m_array[m_size] = i;
    m_size += 1;
  }

  void reserve(size_t n) {
    if(n <= m_capacity)
      return;

    std::unique_ptr<int[]> p(new int[n]);
    if(m_size > 0) {
      memcpy(p.get(), m_array.get(), m_size * sizeof(int))  ;
    }

    m_capacity = n;
    std::swap(p, m_array);
  }

private:
  std::unique_ptr<int[]> m_array;
  size_t m_size;
  size_t m_capacity;
};




template<template<typename> class TQueue>
uint64 benchmark_vector(size_t numValues, size_t capacity)
{
  TQueue<intlist> pipe(capacity);
  std::vector<intlist> source;
  std::vector<intlist> dest;
  dest.reserve(numValues);
  source.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    source.push_back(intlist(64, 0));
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
    intlist tmp;
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

  const intlist s(64, 0);
  for (size_t i = 0; i < dest.size(); ++i)
  {
    if (dest[i] != s) {
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

  Params params = getQueueType(argc, argv, "queue_intarray");
  switch (params.type) {
  case QueueType::Folly:
    run(iterations, numValues, capacity, "Folly", benchmark_vector<folly::ProducerConsumerQueue>, params.filename);
    break;
  case QueueType::FollyOpt:
    run(iterations, numValues, capacity, "FollyOpt", benchmark_vector<folly::AlignedProducerConsumerQueue>, params.filename);
    break;
  case QueueType::Boost:
#ifdef TEETIME_HAS_BOOST
    run(iterations, numValues, capacity, "boost::spsc_queue", benchmark_vector<BoostSpscQueue>, params.filename);
    break;
#else
    std::cout << "boost not available" << std::endl;
    return EXIT_FAILURE;
#endif
  case QueueType::TeeTimePointer:
    std::cout << "values not supported" << std::endl;
    return EXIT_FAILURE;
  case QueueType::TeeTimeValue:
    run(iterations, numValues, capacity, "SpscValueQueue", benchmark_vector<SpscValueQueue>, params.filename);
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
