/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/teetime-framework/TeeTime-Cpp)
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
#include <functional>

namespace teetime
{
  //forward declarations:

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
      typed_out->m_pipe.reset(new SynchedPipe<T, SpscValueQueue>((uint32)capacity));
    }
    else
    {
      typed_out->m_pipe.reset(new UnsynchedPipe<T>(typed_in->owner()));
    }

    typed_in->m_pipe = typed_out->m_pipe.get();
  }
}

  /**
   * A configuration.
   * Derive your P&F configuration from this class.
   * Create your stages in the constructor of that class and connect
   * them by 'connectPorts' and declare at least one stage active with
   * 'declareStageActive'.
   *
   * execute the configuration with 'executeBlocking()';
   */
  class Configuration
  {
  public:
    Configuration();
    virtual ~Configuration();

    //no copy construction, no assignment
    Configuration(const Configuration&) = delete;
    Configuration& operator=(const Configuration&) = delete;

    /**
     * execute configuration. Blocks while configuration is executing.
     */
    void executeBlocking();

  protected:
    //create arbitrary stage
    template<typename T, typename ...TArgs>
    static shared_ptr<T> createStage(TArgs... args)
    {
      auto stage = std::make_shared<T>(args...);
      return stage;
    }

    /**
     * Create a stage from a lambda. Lambda must take one argument (stage input) and must
     * return one element (stage output).
     */
    //jesus! this is just ugly as hell... but it get's the job done (most of the time)
    template<typename T>
    static auto createStageFromLambda(T lambda, const char* debugName = "lambda_stage")
      ->shared_ptr<FunctionObjectStage<typename std::remove_reference<typename teetime::internal::function_traits<T>::arg_type>::type,
      typename teetime::internal::function_traits<T>::result_type>>
    {
      using TOut = typename teetime::internal::function_traits<T>::result_type;
      using TIn = typename std::remove_reference<typename teetime::internal::function_traits<T>::arg_type>::type;
      return createStage<FunctionObjectStage<TIn, TOut>>(lambda, debugName);
    }

    /**
     * Create a stage from a function pointer. Function must take one argument (stage input) and must
     * return one element (stage output).
     */
    template<typename TIn, typename TOut>
    static shared_ptr<NewFunctionStage<TIn, TOut>> createStageFromFunctionPointer(TOut(*f)(TIn), const char* name = "function_pointer")
    {
      return createStage<NewFunctionStage<TIn, TOut>>(f, name);
    }

    /**
     * Create a stage from std::function. Function must take one argument (stage input) and must
     * return one element (stage output).
     */
    template<typename TIn, typename TOut>
    static shared_ptr<FunctionObjectStage<TIn, TOut>> createStageFromFunctionObject(std::function<TOut(TIn)> f, const char* name = "function_object")
    {
      return createStage<FunctionObjectStage<TIn, TOut>>(f, name);
    }

    /**
     * Create a stage from a function itself (not from a pointer to it).
     * Function must take one argument (stage input) and must return one element (stage output).
     */
    template<typename TIn, typename TOut, TOut(*TFunc)(TIn)>
    shared_ptr<FunctionStage<TIn, TOut, TFunc>> createStageFromFunction(const char* name = "function")
    {
      return createStage<FunctionStage<TIn, TOut, TFunc>>(name);
    }

    /**
     * @brief connect an output port to an input port. Connection is automatically
     *        synched if the stages are running in different threads.
     * @param output output port
     * @param input input port
     * @param capacity queue capacity (if connection must be synched by a queue/pipe)
     * @tparam T element type to be passed from output to input
     * @tparam TQueue queue implementation to use for synched pipe
     */
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

    /**
     * Declare stage active.
     * @param stage stage to make active
     * @param cpus bit mask for CPU affinity. If 0, no CPU affinity is used.
     */
    void declareStageActive(shared_ptr<AbstractStage> stage, unsigned cpus = 0);

    /**
     * Declare a stage non-active.
     * @param stage stage to make non-active.
     */
    void declareStageNonActive(shared_ptr<AbstractStage> stage);

    /**
     * Check if the given port is already connected.
     */
    bool isPortConnected(const AbstractInputPort& port) const;

    /**
     * Check if the given port is already connected.
     */
    bool isPortConnected(const AbstractOutputPort& port) const;

  private:
    /**
     * Instantiate all port connections by creating pipes.
     */
    void createConnections();

    /**
     * settings associated with a single stage.
     */
    struct stageSettings
    {
      stageSettings()
        : isActive(false)
        , cpuAffinity(0)
      {}
      bool isActive;
      unsigned cpuAffinity;
    };

    /**
     * Not-yet instantiated pipe.
     */
    struct connection
    {
      AbstractOutputPort* out; //output port
      AbstractInputPort* in; //input port
      size_t capacity; //queue capacity
      internal::CreatePipeCallback* createPipeCallback;
      internal::ConnectCallback* connectCallback;
    };

    //all connections between ports.
    std::vector<connection> m_connections;

    //all stages, that are either active or have been connected
    //we store a shared_ptr to each one of thoses stages to make sure, so we can be sure
    //all stages exist at least as long as the configuration they are used in.
    std::set<shared_ptr<AbstractStage>> m_stages;

    //lookup map to store settings for each stage.
    std::map<AbstractStage*, stageSettings> m_stageSettings;
  };
}