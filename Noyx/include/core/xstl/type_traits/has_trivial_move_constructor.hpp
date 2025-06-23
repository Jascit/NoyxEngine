#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_trivial_move_constructor : bool_constant<__is_trivially_move_constructible(T)>{};

  template<typename T>
  constexpr bool has_trivial_move_constructor_v = has_trivial_move_constructor<T>::value;
}
