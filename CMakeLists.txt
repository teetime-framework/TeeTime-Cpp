# Copyright (C) 2016 Johannes Ohlemacher (https://github.com/teetime-framework/TeeTime-Cpp)
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

cmake_minimum_required (VERSION 3.0.2)
project (TeeTime)

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)
endif()

if(MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

option(TEETIME_ENABLE_CPP17 "enable C++17 compiler features" ON)
option(TEETIME_ENABLE_FILESYSTEM "enable C++17 filesystem support" ON)
option(TEETIME_ENABLE_TESTS "enable unit tests" OFF)
option(TEETIME_ENABLE_BENCHMARKS "enable benchmarks" OFF)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(TEETIME_ENABLE_CPP17 OFF)
  set(TEETIME_ENABLE_FILESYSTEM OFF)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  if (TEETIME_ENABLE_CPP17 AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7.2)
    set(TEETIME_ENABLE_CPP17 ON)
  else()
    set(TEETIME_ENABLE_CPP17 OFF)
  endif()

  if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
    set(TEETIME_ENABLE_FILESYSTEM ON)
  else()
    set(TEETIME_ENABLE_FILESYSTEM OFF)
  endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  if (TEETIME_ENABLE_CPP17 AND MSVC_VERSION GREATER_EQUAL 1914)
    set(TEETIME_ENABLE_CPP17 ON)
  else()
    set(TEETIME_ENABLE_CPP17 OFF)
    set(TEETIME_ENABLE_FILESYSTEM OFF)
  endif()
endif()

message(STATUS "TEETIME_ENABLE_CPP17: ${TEETIME_ENABLE_CPP17}")
message(STATUS "TEETIME_ENABLE_FILESYSTEM: ${TEETIME_ENABLE_FILESYSTEM}")
message(STATUS "TEETIME_ENABLE_TESTS: ${TEETIME_ENABLE_TESTS}")
message(STATUS "TEETIME_ENABLE_BENCHMARKS: ${TEETIME_ENABLE_BENCHMARKS}")

function(set_compile_options targetname)
  if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if (TEETIME_ENABLE_CPP17)
      target_compile_options(${targetname} PUBLIC -std=c++17)
    else()
      target_compile_options(${targetname} PUBLIC -std=c++11)
    endif()

    target_compile_options(${targetname} PRIVATE -Wall -Wpedantic -Wextra -Wunused-value -Wunused-function -Wunused-variable -Wswitch)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:DEBUG>:-g -fno-omit-frame-pointer>)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:RELEASE>:-O3 -DNDEBUG>)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:RELWITHDEBINFO>:-O3 -DNDEBUG -g -fno-omit-frame-pointer>)
  elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if (TEETIME_ENABLE_CPP17)
      target_compile_options(${targetname} PUBLIC /std:c++17)
    endif()

    target_compile_options(${targetname} PUBLIC /std:c++17)
    target_compile_options(${targetname} PUBLIC $<$<CONFIG:DEBUG>:/MTd>)
    target_compile_options(${targetname} PUBLIC $<$<CONFIG:RELEASE>:/MT>)
    target_compile_options(${targetname} PUBLIC $<$<CONFIG:RELWITHDEBINFO>:/MT>)

    target_compile_options(${targetname} PRIVATE /W4 /MP /EHsc /D_CRT_SECURE_NO_WARNINGS /D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:DEBUG>:/Od /D_SECURE_SCL=1 /Zi>)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:RELEASE>:/O2 /DNDEBUG /D_SECURE_SCL=0>)
    target_compile_options(${targetname} PRIVATE $<$<CONFIG:RELWITHDEBINFO>:/O2 /DNDEBUG /D_SECURE_SCL=0 /Zi>)
  endif()

  if (TEETIME_ENABLE_FILESYSTEM)
    target_compile_definitions(${targetname} PUBLIC TEETIME_HAS_FILESYSTEM)
  endif()
endfunction(set_compile_options)

add_subdirectory(src)

if (TEETIME_ENABLE_TESTS)
  add_subdirectory(test)
endif()

IF (TEETIME_ENABLE_BENCHMARKS)
  add_subdirectory(benchmark)
endif()

