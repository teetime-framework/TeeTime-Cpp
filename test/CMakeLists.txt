# Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_subdirectory(googletest/googletest)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET "gtest" PROPERTY FOLDER googletest)
  set_property(TARGET "gtest_main" PROPERTY FOLDER googletest)
endif()

include_directories(googletest/googletest/include)

SET(SOURCES
  stages/IntProducerStage.h
  stages/IntConsumerStage.h
  stages/Directory2FilesTest.cpp
  stages/File2FileBufferTest.cpp
  stages/FileExtensionSwitchTest.cpp
  stages/ReadImageTest.cpp
  stages/Md5HashingTest.cpp
  stages/FunctionStageTest.cpp
  ConfigurationTest.cpp
  DistributorStageTest.cpp
  DelayStageTest.cpp
  MergerStageTest.cpp
  StageTest.cpp
  SynchedPipeTest.cpp
  UnsynchedPipeTest.cpp
  LogTest.cpp
  SpscQueueTest.cpp
  main.cpp
)

add_definitions(-DTEETIME_LOCAL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
add_executable("teetime_test" ${SOURCES})
target_link_libraries(teetime_test gtest teetime)

add_custom_target(run_test ALL teetime_test)
