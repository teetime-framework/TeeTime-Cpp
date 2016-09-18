add_executable(mipmaps ../Benchmark.h mipmaps.cpp mipmaps_teetime.cpp mipmaps_fastflow.cpp mipmaps_fastflow_allocator.cpp)
target_link_libraries(mipmaps teetime ${CMAKE_THREAD_LIBS_INIT})
target_compile_definitions(mipmaps PRIVATE -DTEETIME_MIPMAP_IMAGE="${CMAKE_CURRENT_SOURCE_DIR}/lena.png")