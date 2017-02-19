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

cmake_minimum_required (VERSION 2.8.0)
project (TeeTime)

if(POLICY CMP0054)
  cmake_policy(SET CMP0054 OLD)
endif()

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=c++11   -Wall -Wpedantic -Wextra -Wunused-value -Wunused-function -Wunused-variable -Wswitch")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -fno-omit-frame-pointer -fsanitize=address")
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
  SET(CMAKE_CYY_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG -g -fno-omit-frame-pointer")
ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS                "${CMAKE_CXX_FLAGS} -std=gnu++0x -Wall -Wpedantic -Wextra -Wunused-value -Wunused-function -Wunused-variable -Wswitch")
  SET(CMAKE_CXX_FLAGS_DEBUG          "-g -fno-omit-frame-pointer")
  SET(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
  SET(CMAKE_CYY_FLAGS_RELWITHDEBINFO "-O3 -DNDEBUG -g -fno-omit-frame-pointer")
ELSEIF (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
  SET(CMAKE_CXX_FLAGS                "/W4 /EHsc /D_CRT_SECURE_NO_WARNINGS")
  SET(CMAKE_CXX_FLAGS_DEBUG          "/MTd /Od /D_SECURE_SCL=1 /Zi")
  SET(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /DNDEBUG /D_SECURE_SCL=0")  
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /O2 /DNDEBUG /D_SECURE_SCL=0 /Zi")
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(benchmark)

