#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_trivial_move_assign : bool_constant<__is_trivially_move_assignable(T)>{};

  template<typename T>
  constexpr bool has_trivial_move_assign_v = has_trivial_move_assign<T>::value;
}
