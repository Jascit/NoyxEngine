#pragma once
#include "integral_constant.hpp"
#include "../core/xstldef.hpp"

namespace xstl {
  template<typename>
  struct is_array : false_type{};
  
  template<typename T>
  struct is_array<T[]> : true_type {};
  
  template<typename T, xstl::size_t Ts>
  struct is_array<T[Ts]> : true_type {};

  template<typename T>
  constexpr bool is_array_v = is_array<T>::value;
} // xstl
