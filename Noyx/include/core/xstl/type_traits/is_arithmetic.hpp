#pragma once 
#include <type_traits/integral_constant.hpp>
#include <type_traits/is_integral.hpp>
#include <type_traits/is_floating_point.hpp>

namespace xstl {
  template<typename T>
  struct is_arithmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T>> {};

  template<typename T>
  constexpr bool is_arithmetic_v = is_arithmetic<T>::value;
} // xstl
