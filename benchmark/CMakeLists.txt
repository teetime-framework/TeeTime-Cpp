find_package (Threads)

set(BOOST_ROOT ${CMAKE_SOURCE_DIR}/../boost_1_61_0)
find_package(Boost 1.61.0)
if(Boost_FOUND)
  message(Boost found!)
  add_definitions(-DTEETIME_HAS_BOOST)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

#add_executable("queue" queue.cpp)
#target_link_libraries(queue teetime ${CMAKE_THREAD_LIBS_INIT})

add_executable("md5" Benchmark.h md5.cpp md5_teetime.cpp)
target_link_libraries(md5 teetime ${CMAKE_THREAD_LIBS_INIT})

add_executable("io" Benchmark.h io.cpp io_teetime.cpp)
target_link_libraries(io teetime ${CMAKE_THREAD_LIBS_INIT})

add_subdirectory(mipmaps)
add_subdirectory(queue)

IF(NOT UNIX)
  RETURN()
ENDIF()

add_subdirectory(googlebenchmark)

add_executable("teetime_benchmark" main.cpp)
target_link_libraries(teetime_benchmark ${CMAKE_THREAD_LIBS_INIT} teetime benchmark) #'benchmark' gets defined by googlebenchmark

