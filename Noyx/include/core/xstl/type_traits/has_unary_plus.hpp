#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_unary_plus : xstl::false_type {};

  template<typename T>
    requires requires(T t) { +t; }
  struct has_unary_plus<T> : xstl::true_type {};
}