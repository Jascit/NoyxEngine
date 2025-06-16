#pragma once
#include <type_traits/integral_constant.hpp>
//TODO: Clang, GCC
namespace xstl {
  template<typename From, typename To>
  struct is_convertible : bool_constant<__is_convertible_to(From, To)> {};
  template<typename From, typename To>
  constexpr bool is_convertible_v = is_convertible<From, To>::value;
} // xstl