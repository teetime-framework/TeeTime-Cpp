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
#include <teetime/common.h>
#include <teetime/platform.h>
#include <teetime/logging.h>
#include <teetime/stages/AbstractFilterStage.h>
#include <teetime/stages/InitialElementProducer.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/Configuration.h>

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

#define HAS_CXX11_VARIADIC_TEMPLATES
#include <ff/pipeline.hpp>

TEETIME_WARNING_POP

namespace teetime
{
  class DummyStage : public AbstractFilterStage<int,int>
  {
  public:
    virtual void execute(int&& i) override
    {
      getOutputPort().send(i+1);
    }
  };

  class DummyConfig : public Configuration
  {
  public:
    DummyConfig(int numValues, int numStages)
    {
      std::vector<int> ints(numValues, 0);
      producer = createStage<InitialElementProducer<int>>(ints);
      sink = createStage<CollectorSink<int>>();

      declareStageActive(producer);

      OutputPort<int>* prevOutputPort = &producer->getOutputPort();
      assert(numStages > 0);
      for(int i=0; i<numStages; ++i)
      {
        auto s = createStage<DummyStage>();
        declareStageActive(s);
        connectPorts(*prevOutputPort, s->getInputPort(), 4096);
        prevOutputPort = &s->getOutputPort();
      }

      declareStageActive(sink);

      connectPorts(*prevOutputPort, sink->getInputPort(), 4096);
    }

    std::vector<int> values() const
    {
      return sink->getElements();
    }

  private:
    shared_ptr<InitialElementProducer<int>> producer;
    shared_ptr<CollectorSink<int>> sink;
  };
}

namespace fastflow
{
  using namespace ff;

  struct Producer: ff_node_t<char, int> {
      explicit Producer(int num)
       : num(num)
      {}

      int *svc(char *) {
          for(size_t i=0;i<num;++i)
          {
              int* p = reinterpret_cast<int*>(1);
              ff_send_out(p);
          }
          return EOS;
      }

      int num;
  }; 

  struct Dummy: ff_node_t<int, int> {
      int *svc(int * value) {
        long long i = (long long)value;
        i += 1;
        return reinterpret_cast<int*>(i);
      }
  }; 

  struct Sink: ff_node_t<int, char> {
      Sink()
      {
        data.reserve(1000000);
      }

      char *svc(int * value) {
        data.push_back((int)reinterpret_cast<long long>(value)-1);
        return GO_ON;
      }

      std::vector<int> data;
  }; 
}

void verify(const std::vector<int>& values, size_t expectedSize, int expectedValue)
{
  if(values.size() != expectedSize)
  {
    std::cerr << "size mismatch (" << values.size() << " instead of " << expectedSize << ")" << std::endl;
    return;
  }

  for(size_t i=0; i<values.size(); ++i)
  {
    if(values[i] != expectedValue)
    {
      std::cerr << "value[" << i << "] has wrong value (" << values[i] << " instead of " << expectedValue << ")" << std::endl;
      return;
    }
  }

  std::cerr << "result is ok" << std::endl;
}

int main(int argc, char** argv)
{
  teetime::setLogCallback(::teetime::simpleLogging);
  teetime::setLogLevel(teetime::getLogLevelFromArgs(argc, argv));

  int numValues = 1000000;
  int numStages = 1;
  
  {
    DummyConfig config(numValues, numStages);
    auto start = platform::microSeconds();    
    config.executeBlocking();
    auto end = platform::microSeconds();

    verify(config.values(), numValues, numStages);

    std::cout << "TeeTime time: " << ((end - start) * 0.001) << "ms" << std::endl;
  }

  {
    ff::ff_pipeline pipeline(false, 4096, 4096);
    pipeline.add_stage(new fastflow::Producer(numValues));
    for(int i=0; i<numStages; ++i)
    {
      pipeline.add_stage(new fastflow::Dummy);
    }
    auto sink = new fastflow::Sink();
    pipeline.add_stage(sink);

    auto start = platform::microSeconds();    

    if (pipeline.run_and_wait_end()<0) 
      std::cerr << "failed to run fastflow pipeline";

    auto end = platform::microSeconds();

    verify(sink->data, numValues, numStages);

    std::cout << "FastFlow time: " << ((end - start) * 0.001) << "ms" << std::endl;
  }  

}
