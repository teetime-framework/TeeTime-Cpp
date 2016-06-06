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