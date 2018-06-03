add_executable(queue_vector queue_vector.cpp common.h)
target_link_libraries(queue_vector teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_vector PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_vector)

add_executable(queue_matrix queue_matrix.cpp common.h)
target_link_libraries(queue_matrix teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_matrix PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_matrix)

add_executable(queue_pointer queue_pointer.cpp common.h)
target_link_libraries(queue_pointer teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_pointer PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_pointer)

add_executable(queue_shared queue_shared.cpp common.h)
target_link_libraries(queue_shared teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_shared PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_shared)

add_executable(queue_string queue_string.cpp common.h)
target_link_libraries(queue_string teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_string PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_string)

add_executable(queue_intarray queue_intarray.cpp common.h)
target_link_libraries(queue_intarray teetime ${CMAKE_THREAD_LIBS_INIT})
set_property(TARGET queue_intarray PROPERTY FOLDER benchmarks/queue)
set_compile_options(queue_intarray)

add_executable(stats stats.cpp)
set_property(TARGET stats PROPERTY FOLDER benchmarks/utils)
set_compile_options(stats)