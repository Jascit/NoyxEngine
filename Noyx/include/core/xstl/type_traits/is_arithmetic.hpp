#pragma once 
#include <type_traits/integral_constant.hpp>
#include <type_traits/is_integral.hpp>
#include <type_traits/is_floating_point.hpp>

namespace xstl {
  template<typename T>
  struct is_arihmetic : bool_constant<is_integral_v<T> || is_floating_point_v<T>> {};

  template<typename T>
  using is_arihmetic_v = is_arihmetic<T>::value;
} // xstl
