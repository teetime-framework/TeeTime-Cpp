#include "teetime/logging.h"
#include <iostream>
#include <thread>

using namespace teetime;

namespace
{
  struct LoggingData
  {
    const char* file;
    int line;
    LogLevel level;
    std::stringstream buffer;
  };

  thread_local LoggingData logdata;

  const char* LogLevel2String(LogLevel l)
  {
    switch(l)
    {
    case LogLevel::Trace:
      return "TRACE";
    case LogLevel::Debug:
      return "DEBUG";
    case LogLevel::Info:
      return "INFO";
    case LogLevel::Warn:
      return "WARN";
    case LogLevel::Error:
      return "ERROR";
    default:
      return "???";
    }
  }    
}

Logger::Logger(const char* file, int line, LogLevel level)
{
  logdata.file = file;
  logdata.line = line;
  logdata.level = level;
}

Logger::~Logger()
{
  std::cout << std::this_thread::get_id() << ": " << LogLevel2String(logdata.level) << " " << logdata.file << "(" << logdata.line << "): " << logdata.buffer.str() << std::endl;
  logdata.buffer.str("");
}

std::ostream& Logger::buffer()
{
  return logdata.buffer;
}