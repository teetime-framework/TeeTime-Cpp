add_executable(md5 ../Benchmark.h md5.cpp md5_teetime.cpp)
target_link_libraries(md5 teetime ${CMAKE_THREAD_LIBS_INIT})