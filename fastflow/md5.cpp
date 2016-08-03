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

#include <teetime/Md5Hash.h>

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_UNREFERENCED_PARAMETER
TEETIME_WARNING_DISABLE_EMPTY_BODY
TEETIME_WARNING_DISABLE_MISSING_FIELD_INIT

#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

TEETIME_WARNING_POP



using namespace ff;

using teetime::Md5Hash;

struct Producer: ff_node_t<char, int> 
{ 
  Producer(int num, int min, int max)
    : m_num(num)
    , m_min(min)
    , m_max(max)
  {
  } 

  int* svc(char*) 
  {
    std::mt19937                        generator(0);
    std::uniform_int_distribution<int>  distr(m_min, m_max);

    for (unsigned i = 0; i < m_num; ++i) 
    {
      int value = distr(generator);
      ff_send_out(new int(value));
    }

    return EOS;
  }

private:
  int m_num;
  int m_min;
  int m_max;    
}; 

struct Hasher: ff_node_t<int, Md5Hash> 
{ 
  Md5Hash* svc(int* value) 
  {
    auto hash = Md5Hash::generate(value, sizeof(*value));
    delete value;
    return new Md5Hash(hash);
  }
}; 

struct HashCracker: ff_node_t<Md5Hash, int> 
{ 
  int* svc(Md5Hash* value) 
  {
    int ret = -1;
    for(int i=0; i<INT_MAX; ++i) {
      if( Md5Hash::generate(&i, sizeof(i)) == *value ) {
        ret = i;
        break;
      }
    }    

    delete value;
    return new int(ret);
  }
}; 

struct Sink: ff_node_t<int, char> 
{ 
  virtual char* svc(int* value) override
  {
    m_values.push_back(*value);
    delete value;
    return GO_ON;
  }

  std::vector<int> m_values;
};


struct Deleter: ff_node_t<int,char> 
{
  virtual char *svc(int* value) override
  {
    delete value;
    return GO_ON;
  }
};

int main(int argc, char** argv) 
{
  if(argc < 5) {
    std::cout << "usage: md5 <count> <min> <max> <threads> [verbose]" << std::endl;
    return EXIT_FAILURE;
  }
  
  int num = atoi(argv[1]);
  int min = atoi(argv[2]);
  int max = atoi(argv[3]);
  int threads = atoi(argv[4]);
  bool verbose = (argc > 5 && atoi(argv[5]) > 0);

  std::cout << "settings: num=" << num << ", min=" << min << ", max=" << max << ", threads=" << threads << ", verbose=" << verbose << std::endl;

  Producer producer(num, min, max);
  Hasher hasher;
  HashCracker cracker;
  Sink sink;
  
  ff_Pipe<> pipe(producer, hasher, cracker, sink);      // NOTE: references used here instead of pointers
  
  if (pipe.run_and_wait_end()<0) 
    error("running pipe\n");

  return EXIT_SUCCESS;
}