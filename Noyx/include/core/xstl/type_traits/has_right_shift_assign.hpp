#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_right_shift_assign : xstl::false_type {};

  template<typename T>
    requires requires { xstl::declval<T>() >>= xstl::declval<T>(); }
  struct has_right_shift_assign<T> : xstl::true_type {};

}
