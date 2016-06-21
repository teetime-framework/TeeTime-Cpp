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
#include <gtest/gtest.h>
#include <teetime/logging.h>

::teetime::LogLevel getLogLevelFromArgs( int argc, char** argv ) 
{
  for(int i=0; i<(argc-1); ++i)
  {
    std::cout << "????\n";
    if(strcmp(argv[i], "--loglevel") == 0)
    {
      std::cout << "ADASDASDASDASD\n";
      return ::teetime::String2LogLevel(argv[i+1]);
    }    
  }

  return ::teetime::LogLevel::Off;
}

int main( int argc, char** argv )
{
  ::teetime::setLogCallback(::teetime::simpleLogging);
  ::teetime::setLogLevel(getLogLevelFromArgs(argc, argv));

  ::testing::InitGoogleTest( &argc, argv );

  int ret = RUN_ALL_TESTS();
  return ret;
}