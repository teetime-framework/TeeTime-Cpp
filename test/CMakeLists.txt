add_subdirectory(googletest/googletest)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set_property(TARGET "gtest" PROPERTY FOLDER googletest)
  set_property(TARGET "gtest_main" PROPERTY FOLDER googletest)
endif()

include_directories(googletest/googletest/include)

add_executable("teetime_test" ${HEADERS} footest.cpp)
target_link_libraries(teetime_test gtest teetime)