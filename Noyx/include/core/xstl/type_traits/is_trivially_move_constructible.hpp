#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct is_trivially_move_constructible : bool_constant<__is_trivially_move_constructible(T)> {};
  
  template<typename T>
  constexpr bool is_trivially_move_constructible_v = is_trivially_move_constructible<T>::value;
}