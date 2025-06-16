#pragma once
#include <type_traits/integral_constant.hpp>
//TODO: Clang, GCC
namespace xstl {
  template<typename To, typename From>
  struct is_assignable : bool_constant<__is_assignable(To, From)> {};
  template<typename To, typename From>
  constexpr bool is_assignable_v = is_assignable<To, From>;
} // xstl
