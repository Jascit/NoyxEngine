#pragma once
#include <type_traits/integral_constant.hpp>
//TODO: Clang/GCC 
namespace xstl {
  template<typename T>
  struct is_enum : bool_constant<__is_enum(T)> {};

  template<typename T>
  constexpr bool is_enum_v = is_enum<T>::value;
} // xstl