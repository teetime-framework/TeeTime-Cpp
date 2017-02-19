find_package (Threads)

find_package(Boost 1.54.0)
if(Boost_FOUND)
  add_definitions(-DTEETIME_HAS_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(mipmaps)
add_subdirectory(queue)
add_subdirectory(io)
add_subdirectory(md5)

IF(NOT UNIX)
  RETURN()
ENDIF()

add_subdirectory(googlebenchmark)

add_executable("teetime_benchmark" main.cpp)
target_link_libraries(teetime_benchmark ${CMAKE_THREAD_LIBS_INIT} teetime benchmark) #'benchmark' gets defined by googlebenchmark

