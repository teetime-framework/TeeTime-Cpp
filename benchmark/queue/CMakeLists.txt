add_executable(queue_vector queue_vector.cpp common.h)
target_link_libraries(queue_vector teetime ${CMAKE_THREAD_LIBS_INIT})

add_executable(queue_matrix queue_matrix.cpp common.h)
target_link_libraries(queue_matrix teetime ${CMAKE_THREAD_LIBS_INIT})

add_executable(queue_pointer queue_pointer.cpp common.h)
target_link_libraries(queue_pointer teetime ${CMAKE_THREAD_LIBS_INIT})

add_executable(stats stats.cpp)

