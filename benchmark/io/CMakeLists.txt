add_executable(io ../Benchmark.h io.cpp io_teetime.cpp)
target_link_libraries(io teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET io PROPERTY FOLDER benchmarks)
set_compile_options(io)