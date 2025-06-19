#pragma once
#include <type_traits/integral_constant.hpp>
//TODO: Clang, GCC
namespace xstl {
  template<typename T>
  struct is_destructible : bool_constant<__is_destructible(T)>{};
  template<typename T>
  constexpr bool is_destructible_v = is_destructible<T>::value;
} // xstl