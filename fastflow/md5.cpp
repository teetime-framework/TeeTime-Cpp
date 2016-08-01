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
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>
using namespace ff;

int main() {

  static std::string ExNumber = "1";

  // generates 10 tasks
  struct Stage0: ff_node_t<char, long> {  // NOTE: 'void' cannot be used as input/output type!
      long *svc(char *) {
          for(size_t i=0;i<10;++i) {
              auto p = new long(i);
              std::cout << std::this_thread::get_id() << " (" << __LINE__ << ")  out: " << *p << "\n";
              ff_send_out(p);
          }
          return EOS;
      }
  };	
  // prints tasks
  struct Stage1: ff_node_t<long,char> {   // NOTE: 'void' cannot be used as input/output type!
      int svc_init() { std::cout << "Ex " << ExNumber << ":\n"; return 0;}
      char *svc(long *task) {
          std::cout << std::this_thread::get_id() << *task << "\n" ;
          delete task;
          return GO_ON;
      }
      void svc_end() { std::cout << "\n----------\n"; }
  };
  // function: it adds 1 to the input task and then returns the task
  auto F = [](long *in, ff_node*const)->long* {
      std::cout << std::this_thread::get_id() << " (" << __LINE__ << ")  in: " << *in << "\n";
      *in += 1;
      return in;
  };

  Stage0 s0;
  Stage1 s1;
  
  // creating stages from a function F
  ff_node_F<long,long> s01(F), s011(F);
  
  // creates a farm from a function F with 3 workers, 
  // default emitter and collector implicitly added to the farm
  //ff_Farm<long,long> farm(F,3);
  //          ^
  //          |----- here the types have to be set because the 'farm' will be used in a pipeline (see below). 
  
  // pipe with a farm
  //ff_Pipe<long,long> pipe0(s01,s011, farm); // NOTE: references used here instead of pointers
  //          ^
  //          |----- here the types have to be set because the 'pipe0' will be used as a stage of another pipeline 
  
  // main pipe, no input and output stream
  ff_Pipe<> pipe(s0, s1);      // NOTE: references used here instead of pointers
  
  if (pipe.run_and_wait_end()<0) 
    error("running pipe\n");

}
