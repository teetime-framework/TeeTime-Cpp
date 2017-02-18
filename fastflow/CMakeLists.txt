# Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#         http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


add_definitions(-DHAS_CXX11_VARIADIC_TEMPLATES)
include_directories(fastflow)
include_directories(../include)

find_package (Threads)

add_executable("fastflow_md5" "md5.cpp")

target_link_libraries(fastflow_md5 ${CMAKE_THREAD_LIBS_INIT} teetime)
