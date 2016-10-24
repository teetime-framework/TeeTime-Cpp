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
#pragma once
#include "stages/AbstractStage.h"
#include "pipes/SpscValueQueue.h"
#include "pipes/SynchedPipe.h"
#include "pipes/UnsynchedPipe.h"
#include <map>
#include <set>
#include <type_traits>
namespace teetime
{
  template<typename TIn, typename TOut, TOut(*TFunc)(TIn)>
  class FunctionStage;

  template<typename TIn, typename TOut>
  class FunctionPtrStage;

  template<typename TIn, typename TOut>
  class FunctionObjectStage;

  template<typename TIn, typename TOut>
  class NewFunctionStage;

namespace internal
{
  template <typename T>
  struct function_traits
    : public function_traits<decltype(&T::operator())>
  {};

  template <typename ClassType, typename ReturnType, typename Arg>
  struct function_traits<ReturnType(ClassType::*)(Arg) const>
  {
    using result_type = ReturnType;
    using arg_type = Arg;
  };

  template <typename ClassType, typename ReturnType, typename Arg>
  struct function_traits<ReturnType(ClassType::*)(Arg)>
  {
    using result_type = ReturnType;
    using arg_type = Arg;
  };


  using CreatePipeCallback = void*(size_t capacity, bool synched);
  using ConnectCallback = void(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched);

  template<typename T>
  void connectPortsCallback(AbstractOutputPort* out, AbstractInputPort* in, size_t capacity, bool synched)
  {
    auto typed_out = unsafe_dynamic_cast<OutputPort<T>>(out);
    auto typed_in = unsafe_dynamic_cast<InputPort<T>>(in);

    if (synched)
    {
      typed_out->m_pipe.reset(new SynchedPipe<T, v2::SpscValueQueue>((uint32)capacity));
    }
    else
    {
      typed_out->m_pipe.reset(new UnsynchedPipe<T>(typed_in->owner()));
    }

    typed_in->m_pipe = typed_out->m_pipe.get();
  }
}

  class Configuration
  {



  public:
    Configuration();
    virtual ~Configuration();

    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    void executeBlocking();

  protected:
    template<typename T, typename ...TArgs>
    static shared_ptr<T> createStage(TArgs... args)
    {
      auto stage = std::make_shared<T>(args...);
      return stage;
    }

    //jesus... this is just ugly as hell... but it get's the job done (most of the time)
    template<typename T>
    static auto createStageFromLambda(T lambda, const char* debugName = "lambda_stage")
      ->shared_ptr<FunctionObjectStage<typename teetime::internal::function_traits<T>::arg_type,
      typename teetime::internal::function_traits<T>::result_type>>
    {
      using TOut = typename teetime::internal::function_traits<T>::result_type;
      using TIn = typename teetime::internal::function_traits<T>::arg_type;
      return createStage<FunctionObjectStage<TIn, TOut>>(lambda, debugName);
    }

    template<typename TIn, typename TOut>
    static shared_ptr<NewFunctionStage<TIn, TOut>> createStageFromFunctionPointer(TOut(*f)(TIn), const char* name = "function_pointer")
    {
      return createStage<NewFunctionStage<TIn, TOut>>(f, name);
    }

    template<typename TIn, typename TOut>
    static shared_ptr<FunctionObjectStage<TIn, TOut>> createStageFromFunctionObject(std::function<TOut(TIn)> f, const char* name = "function_object")
    {
      return createStage<FunctionObjectStage<TIn, TOut>>(f, name);
    }

    template<typename TIn, typename TOut, TOut(*TFunc)(TIn)>
    shared_ptr<FunctionStage<TIn, TOut, TFunc>> createStageFromFunction(const char* name = "function")
    {
      return createStage<FunctionStage<TIn, TOut, TFunc>>(name);
    }


    template<typename T, template<typename> class TQueue = SpscValueQueue>
    void connectPorts(OutputPort<T>& output, InputPort<T>& input, size_t capacity = 1024)
    { 
      if (isPortConnected(output)) {
        throw std::logic_error("output port is already connected");
      }

      if (isPortConnected(input)) {
        throw std::logic_error("input port is already connected");
      }

      //insert connection
      connection ca;
      ca.in = &input;
      ca.out = &output;
      ca.capacity = capacity;
      ca.connectCallback = &internal::connectPortsCallback<T>;
      m_connections.push_back(ca);

      //make sure, both stages are known to the configuration
      m_stages.insert(output.owner()->shared_from_this());
      m_stages.insert(input.owner()->shared_from_this());
    }

    void declareStageActive(shared_ptr<AbstractStage> stage, uint64 cpus = 0);
    void declareStageNonActive(shared_ptr<AbstractStage> stage);
    
    bool isPortConnected(const AbstractInputPort& port) const;
    bool isPortConnected(const AbstractOutputPort& port) const;

  private:  
    void createConnections();

    struct stageSettings
    {
      stageSettings()
        : isActive(false)
        , cpuAffinity(0)
      {}
      bool isActive;
      uint64 cpuAffinity;
    };

    struct connection
    {
      AbstractOutputPort* out;
      AbstractInputPort* in;
      size_t capacity;
      internal::CreatePipeCallback* createPipeCallback;
      internal::ConnectCallback* connectCallback;
    };
    
    std::vector<connection> m_connections;

    std::set<shared_ptr<AbstractStage>> m_stages;
    std::map<AbstractStage*, stageSettings> m_stageSettings;    
  };
}