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

set_property(TARGET "gtest" PROPERTY FOLDER googletest)
set_property(TARGET "gtest_main" PROPERTY FOLDER googletest)

add_library(test_main main.cpp)
target_link_libraries(test_main gtest teetime)
set_property(TARGET test_main PROPERTY FOLDER tests)
set_compile_options(test_main)

function(add_unit_test filename)
  get_filename_component(testname "${filename}" NAME_WE)
  get_filename_component(testdir "${filename}" DIRECTORY)
  add_executable(${testname} ${filename})
  target_link_libraries(${testname} test_main)
  add_test(${testname} ${testname})
  set_property(TARGET ${testname} PROPERTY FOLDER "tests/${testdir}")
  set_compile_options(${testname})

  add_custom_target(run_${testname} ALL ${testname})
  set_property(TARGET run_${testname} PROPERTY FOLDER "run-tests/${testdir}")
endfunction(add_unit_test)

add_definitions(-DTEETIME_LOCAL_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_unit_test(stages/Directory2FilesTest.cpp)
add_unit_test(stages/File2FileBufferTest.cpp)
add_unit_test(stages/FileExtensionSwitchTest.cpp)
add_unit_test(stages/ReadImageTest.cpp)
add_unit_test(stages/Md5HashingTest.cpp)
add_unit_test(stages/FunctionStageTest.cpp)
add_unit_test(ConfigurationTest.cpp)
add_unit_test(DistributorStageTest.cpp)
add_unit_test(DelayStageTest.cpp)
add_unit_test(MergerStageTest.cpp)
add_unit_test(StageTest.cpp)
add_unit_test(SynchedPipeTest.cpp)
add_unit_test(UnsynchedPipeTest.cpp)
add_unit_test(LogTest.cpp)
add_unit_test(SpscQueueTest.cpp)

enable_testing()



