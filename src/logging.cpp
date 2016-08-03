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
#include "teetime/logging.h"
#include "teetime/common.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <iomanip>
#include <cstring>
#include <mutex>

using namespace teetime;

namespace
{
  struct LogData
  {
    const char* file;
    int line;
    LogLevel level;
    std::stringstream buffer;
  };

  thread_local LogData* logdata = nullptr;

  LogLevel currentLogLevel = LogLevel::Off;
  LogCallback logCallback = nullptr;
  void* customLogCallbackData = nullptr;
}

namespace teetime
{
  void setLogCallback(LogCallback cb, void* customData)
  {
    logCallback = cb;
    customLogCallbackData = customData;
  }

  void setLogLevel(LogLevel level)
  {
    currentLogLevel = level;
  }

  bool isLogEnabled(LogLevel level)
  {
    return logCallback && ((int)level >= (int)currentLogLevel);
  }

  const char* LogLevel2String(LogLevel l)
  {
    switch(l)
    {
    case LogLevel::All:
      return "ALL";
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
    case LogLevel::Off:
      return "OFF";      
    default:
      return "???";
    }
  } 

  LogLevel String2LogLevel(const char* s) 
  {
    if(!s)
    {
      return LogLevel::Off;
    }

    for(int i=0; i<(int)LogLevel::COUNT; ++i)
    {
      auto level = static_cast<LogLevel>(i);

      if(strcmp(s, LogLevel2String(level)) == 0)
      {
        return level;
      }          
    }

    return LogLevel::Off;
  }

  void simpleLogging(std::thread::id threadid, const char* file, int line, LogLevel level, const char* message, void* customData)  
  {
    unused(customData);

    static std::mutex mutex;

    std::lock_guard<std::mutex> lock(mutex);

    std::cout << threadid << ": " << LogLevel2String(level) << " ";

    static const int filenameFieldWidth = 30;
    const int filenameLen = strlen(file);
    if(filenameLen > filenameFieldWidth)
    {
      std::cout.write( "...", 3 );
      std::cout.write( &file[(filenameLen - filenameFieldWidth)+3], filenameFieldWidth-3 );
    }
    else
    {
      std::cout << std::setw(filenameFieldWidth) << file;
    }

    std::cout << "(" << line << "): " << message << std::endl;
  }

  LogLevel getLogLevelFromArgs( int argc, char** argv ) 
  {
    for(int i=0; i<(argc-1); ++i)
    {
      if(strcmp(argv[i], "--loglevel") == 0)
      {
        return String2LogLevel(argv[i+1]);
      }    
    }

    return LogLevel::Off;
  }  
}


Logger::Logger(const char* file, int line, LogLevel level)
{
	if (!logdata)
		logdata = new LogData();

  logdata->file = file;
  logdata->line = line;
  logdata->level = level;
}

Logger::~Logger()
{ 
  assert(logdata);
  logCallback(std::this_thread::get_id(), logdata->file, logdata->line, logdata->level, logdata->buffer.str().c_str(), customLogCallbackData);
  logdata->buffer.str("");    
}

std::ostream& Logger::buffer()
{
  assert(logdata);
  return logdata->buffer;
}

