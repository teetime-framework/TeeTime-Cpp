#include "teetime/logging.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <cstring>
#include <mutex>

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
      return "INFO ";
    case LogLevel::Warn:
      return "WARN ";
    case LogLevel::Error:
      return "ERROR";
    default:
      return " ??? ";
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
  static std::mutex mutex;

  std::lock_guard<std::mutex> lock(mutex);

  std::cout << std::this_thread::get_id() << ": " << LogLevel2String(logdata.level) << " ";

  static const int filenameFieldWidth = 30;
  const int filenameLen = strlen(logdata.file);
  if(filenameLen > filenameFieldWidth)
  {
    std::cout.write( "...", 3 );
    std::cout.write( &logdata.file[(filenameLen - filenameFieldWidth)+3], filenameFieldWidth-3 );
  }
  else
  {
    std::cout << std::setw(filenameFieldWidth) << logdata.file;
  }

  std::cout << "(" << logdata.line << "): " << logdata.buffer.str() << "\n";
  logdata.buffer.str("");
}

std::ostream& Logger::buffer()
{
  return logdata.buffer;
}
