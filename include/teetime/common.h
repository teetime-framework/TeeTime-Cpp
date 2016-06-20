#pragma once
#include <cassert>
#include <memory>
#include "logging.h"

namespace teetime
{
  using std::shared_ptr;
  using std::unique_ptr;

  using uint32 = unsigned;
  using int32 = int;

  template<typename S, typename T>
  T* semi_dynamic_cast (S* s)
  {
    assert(dynamic_cast<T*>(s) != nullptr);
    return reinterpret_cast<T*>(s);
  }
}