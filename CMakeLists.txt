cmake_minimum_required (VERSION 2.8.11)
project (TeeTime)

IF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11   -Wall -Wpedantic -Wextra -Wunused-value -Wunused-function -Wunused-variable -Wswitch")
  SET(CMAKE_CXX_FLAGS_DEBUG   "-g -fno-omit-frame-pointer -fsanitize=address")
  SET(CMAKE_CXX_FLAGS_RELEASE "-g -O3")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=gnu++0x -Wall -Wpedantic -Wextra -Wunused-value -Wunused-function -Wunused-variable -Wswitch")
  SET(CMAKE_CXX_FLAGS_DEBUG   "-g -fno-omit-frame-pointer")
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  SET(CMAKE_CXX_FLAGS "/W4 /EHsc")
  SET(CMAKE_CXX_FLAGS_DEBUG   "/MTd /Od /Zi")
  SET(CMAKE_CXX_FLAGS_RELEASE "/MT /O2")    
  SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
ENDIF()

include_directories(include)

add_subdirectory(3rdParty)
add_subdirectory(src)

add_subdirectory(test)
add_subdirectory(benchmark)

