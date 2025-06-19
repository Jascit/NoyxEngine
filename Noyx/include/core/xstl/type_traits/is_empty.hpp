#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct is_empty : bool_constant<__is_empty(T)>{};
  
  template<typename T>
  constexpr bool is_empty_v = is_empty<T>::value;
}