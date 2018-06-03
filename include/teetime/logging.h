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
#include <thread>

#define TEETIME_TRACE() if(teetime::isLogEnabled(teetime::LogLevel::Trace)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Trace)
#define TEETIME_DEBUG() if(teetime::isLogEnabled(teetime::LogLevel::Debug)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Debug)
#define TEETIME_INFO() if(teetime::isLogEnabled(teetime::LogLevel::Info)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Info)
#define TEETIME_WARN() if(teetime::isLogEnabled(teetime::LogLevel::Warn)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Warn)
#define TEETIME_ERROR() if(teetime::isLogEnabled(teetime::LogLevel::Error)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Error)

namespace teetime
{
  enum class LogLevel;

  using LogCallback = void(*)(std::thread::id threadid, const char* file, int line, LogLevel level, const char* message, void* customData);
  void setLogCallback(LogCallback cb, void* customData = nullptr);
  void setLogLevel(LogLevel level);

  /**
   * @brief
   *    Simple logging function that can be used as a log callback.
   * @details
   *    Writes log messages in a threadsafe manner to std out. Only meant to be used for simple uses cases and for
   *    development and debugging of TeeTime itself.
   *    Does not provide sophisticated configuration or formatting options. Pretty slow.
   */
  void simpleLogging(std::thread::id threadid, const char* file, int line, LogLevel level, const char* message, void* customData);

  enum class LogLevel
  {
    All,
    Trace,
    Debug,
    Info,
    Warn,
    Error,
    Off,
    COUNT
  };

  LogLevel getLogLevelFromArgs( int argc, char** argv );

  LogLevel String2LogLevel(const char* s);

  const char* LogLevel2String(LogLevel level);

  bool isLogEnabled(LogLevel level);

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