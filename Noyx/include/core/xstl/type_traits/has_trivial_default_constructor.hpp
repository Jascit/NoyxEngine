#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_trivial_default_constructor : bool_constant<__is_trivially_constructible(T)> {};

  template<typename T>
  constexpr bool has_trivial_default_constructor_v = has_trivial_default_constructor<T>
}
