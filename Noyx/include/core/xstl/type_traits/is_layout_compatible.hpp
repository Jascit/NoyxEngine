#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T, typename U>
  struct is_layout_compatible : bool_constant<__is_layout_compatible(T, U)>{};
  
  template<typename T, typename U>
  constexpr bool is_layout_compatible_v = is_layout_compatible<T, U>::value;
}
