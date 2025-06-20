#pragma once
#include "integral_constant.hpp"
//TODO: Clang/GCC 
namespace xstl {
  template<typename T>
  struct is_union : bool_constant<__is_union(T)> {};

  template<typename T>
  constexpr bool is_union_v = is_union<T>::value;
} // xstl
