#pragma once
#include "is_same.hpp"

namespace xstl {
  template<typename T, typename U>
  concept same_as = is_same_v<T, U>&& is_same_v<U, T>;
}
