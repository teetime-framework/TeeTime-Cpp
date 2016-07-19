IF(NOT UNIX)
  RETURN()
ENDIF()

add_subdirectory(googlebenchmark)

add_executable("teetime_benchmark" main.cpp)
target_link_libraries(teetime_benchmark ${CMAKE_THREAD_LIBS_INIT} teetime benchmark) #'benchmark' gets defined by googlebenchmark
