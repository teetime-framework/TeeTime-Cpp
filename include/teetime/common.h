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
  S* unsafe_dynamic_cast(T* t)
  {
    assert(dynamic_cast<S*>(t));
    return reinterpret_cast<S*>(t);
  }
}