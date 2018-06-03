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
#include <gtest/gtest.h>
#include <teetime/logging.h>

using namespace teetime;

TEST( LogLevelTest, toString )
{
  ASSERT_STREQ(LogLevel2String(LogLevel::All), "ALL");
  ASSERT_STREQ(LogLevel2String(LogLevel::Trace), "TRACE");
  ASSERT_STREQ(LogLevel2String(LogLevel::Debug), "DEBUG");
  ASSERT_STREQ(LogLevel2String(LogLevel::Info), "INFO");
  ASSERT_STREQ(LogLevel2String(LogLevel::Warn), "WARN");
  ASSERT_STREQ(LogLevel2String(LogLevel::Error), "ERROR");
  ASSERT_STREQ(LogLevel2String(LogLevel::Off), "OFF");
}

TEST( LogLevelTest, fromString )
{
  EXPECT_EQ(LogLevel::All, String2LogLevel("ALL"));
  EXPECT_EQ(LogLevel::Trace, String2LogLevel("TRACE"));
  EXPECT_EQ(LogLevel::Debug, String2LogLevel("DEBUG"));
  EXPECT_EQ(LogLevel::Info, String2LogLevel("INFO"));
  EXPECT_EQ(LogLevel::Warn, String2LogLevel("WARN"));
  EXPECT_EQ(LogLevel::Error, String2LogLevel("ERROR"));
  EXPECT_EQ(LogLevel::Off, String2LogLevel("OFF"));
}

namespace
{
  struct LogEntry
  {
    std::string message;
    std::string file;
    int line;
    std::thread::id tid;
    LogLevel level;
  };

  std::vector<LogEntry> entries;

  void testLoggingCallBack(std::thread::id threadid, const char* file, int line, LogLevel level, const char* message, void* customData)
  {
    LogEntry entry;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.tid = threadid;
    entry.level = level;

    ASSERT_EQ((void*)&entries, customData);

    reinterpret_cast<std::vector<LogEntry>*>(customData)->push_back(entry);
  }
}

TEST( LogCallbackTest, callbackAll )
{
  entries.clear();
  ::teetime::setLogCallback(testLoggingCallBack, &entries);
  ::teetime::setLogLevel(::teetime::LogLevel::All);

  TEETIME_TRACE() << "Trace";
  TEETIME_DEBUG() << "Debug";
  TEETIME_INFO() << "Info";
  TEETIME_WARN() << "Warn";
  TEETIME_ERROR() << "Error";

  EXPECT_EQ(size_t(5), entries.size());

  ::teetime::setLogCallback(::teetime::simpleLogging);
}

TEST( LogCallbackTest, callbackInfo )
{
  entries.clear();
  ::teetime::setLogCallback(testLoggingCallBack, &entries);
  ::teetime::setLogLevel(::teetime::LogLevel::Info);

  TEETIME_TRACE() << "Trace";
  TEETIME_DEBUG() << "Debug";
  TEETIME_INFO() << "Info";
  TEETIME_WARN() << "Warn";
  TEETIME_ERROR() << "Error";

  EXPECT_EQ(size_t(3), entries.size());

  ::teetime::setLogCallback(::teetime::simpleLogging);
}

TEST( LogCallbackTest, callbackNull )
{
  entries.clear();
  ::teetime::setLogCallback(nullptr);
  ::teetime::setLogLevel(::teetime::LogLevel::Info);

  TEETIME_TRACE() << "Trace";
  TEETIME_DEBUG() << "Debug";
  TEETIME_INFO() << "Info";
  TEETIME_WARN() << "Warn";
  TEETIME_ERROR() << "Error";

  EXPECT_EQ(size_t(0), entries.size());

  ::teetime::setLogCallback(::teetime::simpleLogging);
}