#pragma once
#include <sstream>

#define TEETIME_INFO() if(teetime::isLoggingEnabled(teetime::LogLevel::Info)) teetime::Logger(__FILE__, __LINE__, teetime::LogLevel::Info)

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

  class Logger
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