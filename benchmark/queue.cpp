#include <teetime/common.h>
#include <teetime/platform.h>
#include <teetime/pipes/ProducerConsumerQueue.h>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
  size_t num = 1000000000L;
  
  folly::ProducerConsumerQueue<size_t> pipe(1024);
  std::vector<size_t> dest;
  dest.reserve(num);

  auto produce = [&](){ 
    const size_t local_num = num;

    for(size_t i=0; i<local_num; ++i)
    {
      while(true)
      {
        if(pipe.write(i))
          break;
      }
    }
  };

  auto consume = [&](){
    const size_t local_num = num;

    for(size_t i=0; i<local_num; ++i)
    {
      size_t val;
      while(true)
      {
        if(pipe.read(val))
        {
          dest.push_back(val);
          break;
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

  std::cout << "dest.size(): " << dest.size() << std::endl;

  for(size_t i=0; i<dest.size(); ++i)
  {
    if(dest[i] != i) {
      hasError = true;
      std::cout << "ERROR: " << i << ":" << dest[i] << std::endl;
      break;
    }
  }  

  std::cout << "time: " << (end - start) * 0.001 << "ms (" << hasError << ")" << std::endl;

}