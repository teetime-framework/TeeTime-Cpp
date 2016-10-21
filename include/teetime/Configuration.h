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
#include <map>
#include <set>

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


    template<typename T>
    void connect(OutputPort<T>& output, InputPort<T>& input, size_t capacity = 1024)
    { 
      if (isConnected(output)) {
        throw std::logic_error("output port is already connected");
      }

      if (isConnected(input)) {
        throw std::logic_error("input port is already connected");
      }

      //insert connection
      connection ca;
      ca.in = &input;
      ca.out = &output;
      ca.capacity = capacity;
      ca.callback = &connectPortsCallback<T>;
      m_connections.push_back(ca);

      //make sure, both stages are known to the configuration
      m_stages.insert(output.owner()->shared_from_this());
      m_stages.insert(input.owner()->shared_from_this());
    }

    void declareActive(shared_ptr<AbstractStage> stage, uint64 cpus = 0);
    void declareNonActive(shared_ptr<AbstractStage> stage);
    
    bool isConnected(const AbstractInputPort& port) const;
    bool isConnected(const AbstractOutputPort& port) const;

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

    using ConnectCallback = void(AbstractOutputPort* out, AbstractInputPort* in, unsigned capacity, bool synched);

    struct connection
    {
      AbstractOutputPort* out;
      AbstractInputPort* in;
      size_t capacity;
      ConnectCallback* callback;
    };
    
    std::vector<connection> m_connections;

    std::set<shared_ptr<AbstractStage>> m_stages;
    std::map<AbstractStage*, stageSettings> m_stageSettings;    
  };
}