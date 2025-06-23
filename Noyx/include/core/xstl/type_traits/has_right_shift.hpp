#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_right_shift : xstl::false_type{};

  template<typename T>
    requires requires (T a, T b) { a >> b; }
  struct has_right_shift<T> : xstl::true_type{};
}