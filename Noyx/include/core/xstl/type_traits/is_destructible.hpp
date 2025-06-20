#pragma once
#include "integral_constant.hpp"
#include "is_complete.hpp"
//TODO: Clang, GCC
namespace xstl {
  template<typename T, typename = void>
  struct is_destructible : bool_constant<false>{};
  template<typename T>
  requires is_complete_v<T>
  struct is_destructible<T, void> : bool_constant<__is_destructible(T)>{};
  template<typename T>
  constexpr bool is_destructible_v = is_destructible<T>::value;
} // xstl
