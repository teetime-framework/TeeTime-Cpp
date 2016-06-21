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
#include <ostream>

#define TEETIME_TRACE() if(teetime::isLoggingEnabled(teetime::LogLevel::Trace)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Trace)
#define TEETIME_DEBUG() if(teetime::isLoggingEnabled(teetime::LogLevel::Debug)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Debug)
#define TEETIME_INFO() if(teetime::isLoggingEnabled(teetime::LogLevel::Info)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Info)
#define TEETIME_WARN() if(teetime::isLoggingEnabled(teetime::LogLevel::Warn)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Warn)
#define TEETIME_ERROR() if(teetime::isLoggingEnabled(teetime::LogLevel::Error)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Error)

namespace teetime
{
  enum class LogLevel
  {
    All,
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    None
  };

  inline bool isLoggingEnabled(LogLevel)
  {
    return true;
  }

  class Logger final
  {
  public:
    Logger(const char* file, int line, LogLevel level);
    ~Logger();

    template<typename T>
    Logger& operator<<(const T& t)
    {
      buffer() << t;
      return *this;
    }

  private:
    std::ostream& buffer();
  };
}