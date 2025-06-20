#pragma once
#include "integral_constant.hpp"
//TODO: GCC, Clang
namespace xstl {
  template<typename T, typename U>
  struct is_pointer_interconvertible_base_of : bool_constant<__is_pointer_interconvertible_base_of(T, U)>{};
  
  template<typename T, typename U>
  constexpr bool is_pointer_interconvertible_base_of_v = is_pointer_interconvertible_base_of<T, U>::value;
}
