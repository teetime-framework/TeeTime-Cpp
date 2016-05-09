IF(NOT UNIX)
  RETURN()
ENDIF()

add_subdirectory(googlebenchmark)

add_executable("mybenchmark" main.cpp)
target_link_libraries(mybenchmark ${CMAKE_THREAD_LIBS_INIT} benchmark) #'benchmark' gets defined by googlebenchmark
