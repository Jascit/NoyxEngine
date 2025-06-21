#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct is_trivially_copyable : bool_constant<__is_trivially_copyable(T)>{};
  
  template<typename T>
  constexpr bool is_trivially_copyable_v = is_trivially_copyable<T>::value;
}
