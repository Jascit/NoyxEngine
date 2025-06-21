#pragma once
#include "integral_constant.hpp"
//TODO: Clang, GCC
namespace xstl {
  template<typename Base, typename Derived >
  struct is_base_of : bool_constant<__is_base_of(Base, Derived)> {};
  template<typename Base, typename Derived >
  constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;
} // xstl
