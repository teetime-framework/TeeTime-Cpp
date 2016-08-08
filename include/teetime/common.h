/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <cassert>
#include <memory>
#include "logging.h"

#ifdef _MSC_VER

#if (_MSC_VER < 1900) 
//MSVC versions prior to 2015 do not support C++11's 'thread_local' keyword
//'__declspec(thread)' achieves the same thing, but does not work for types with a non-trivial ctor ;(
#define thread_local __declspec(thread)
#endif

//pragmas 'managed' and 'unmanaged' are meaningful only when compiled with '/clr[:option]', we can ignore that everywhere
__pragma(warning(disable: 4949))

#define TEETIME_WARNING_PUSH __pragma(warning( push ))
#define TEETIME_WARNING_POP __pragma(warning( pop ))

#define TEETIME_WARNING_DISABLE_CONSTANT_CONDITION __pragma(warning( disable: 4127 ))
#define TEETIME_WARNING_DISABLE_UNREFERENCED_PARAMETER __pragma(warning( disable: 4100 4189 ))
#define TEETIME_WARNING_DISABLE_LOSSY_CONVERSION __pragma(warning( disable: 4244 4309 ))
#define TEETIME_WARNING_DISABLE_UNREACHABLE __pragma(warning( disable: 4702 ))
#define TEETIME_WARNING_DISABLE_EMPTY_BODY
#define TEETIME_WARNING_DISABLE_MISSING_FIELD_INIT
#define TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT __pragma(warning( disable: 4324 ))
#define TEETIME_WARNING_DISABLE_MAY_NOT_ALIGNED __pragma(warning( disable: 4316 ))
#define TEETIME_WARNING_DISABLE_SIGNED_UNSIGNED_MISMATCH __pragma(warning( disable: 4245 ))
#define TEETIME_WARNING_DISABLE_HIDDEN __pragma(warning( disable: 4458 4456 ))
#define TEETIME_WARNING_DISABLE_UNSAFE_USE_OF_BOOL __pragma(warning( disable: 4804 ))

#elif defined(__GNUC__)

#define TEETIME_WARNING_PUSH _Pragma("GCC diagnostic push")
#define TEETIME_WARNING_POP _Pragma("GCC diagnostic pop")

#define TEETIME_WARNING_DISABLE_CONSTANT_CONDITION
#define TEETIME_WARNING_DISABLE_UNREFERENCED_PARAMETER \
 _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
 _Pragma("GCC diagnostic ignored \"-Wunused-variable\"")
#define TEETIME_WARNING_DISABLE_LOSSY_CONVERSION
#define TEETIME_WARNING_DISABLE_UNREACHABLE
#define TEETIME_WARNING_DISABLE_EMPTY_BODY _Pragma("GCC diagnostic ignored \"-Wempty-body\"")
#define TEETIME_WARNING_DISABLE_MISSING_FIELD_INIT _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")
#define TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT
#define TEETIME_WARNING_DISABLE_SIGNED_UNSIGNED_MISMATCH
#define TEETIME_WARNING_DISABLE_MAY_NOT_ALIGNED
#define TEETIME_WARNING_DISABLE_HIDDEN
#define TEETIME_WARNING_DISABLE_UNSAFE_USE_OF_BOOL
#endif

namespace teetime
{
  using std::shared_ptr;
  using std::unique_ptr;

  using uint8 = unsigned char;
  using int8 = signed char;
  using uint32 = unsigned int;
  using int32 = signed int;

  template<typename S, typename T>
  S* unsafe_dynamic_cast(T* t)
  {
    assert(dynamic_cast<S*>(t));
    return reinterpret_cast<S*>(t);
  }

  template<typename T>
  void unused(const T&)
  {
    //do nothing
  }
}