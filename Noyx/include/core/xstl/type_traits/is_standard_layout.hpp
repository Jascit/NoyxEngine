#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct is_standard_layout : bool_constant<__is_standard_layout(T)>{};
  
  template <typename T>
  constexpr bool is_standard_layout_v = is_standard_layout<T>::value;
}
