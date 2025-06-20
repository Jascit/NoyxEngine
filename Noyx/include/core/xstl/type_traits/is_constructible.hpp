#pragma once 
#include <type_traits/integral_constant.hpp>
// TODO: Clang, GCC
namespace xstl {
  template<typename T, typename... Args>
  struct is_constructible : bool_constant<__is_constructible(T, Args...)> {};
  template<typename T, typename... Args>
  constexpr bool is_constructible_v = is_constructible<T, Args...>::value;
} // xstl