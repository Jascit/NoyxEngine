#pragma once
#include <integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct is_same<T, T> : public true_type {};

  template<typename T, typename U>
  struct is_same : public true_type {};
}
