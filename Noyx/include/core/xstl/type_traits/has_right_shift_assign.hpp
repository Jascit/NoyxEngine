#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T, typename U>
  struct has_right_shift_assign : xstl::false_type {};

  template<typename T, typename U>
    requires requires (T a, U b) { a >>= b; }
  struct has_right_shift_assign<T, U> : xstl::true_type {};
}
