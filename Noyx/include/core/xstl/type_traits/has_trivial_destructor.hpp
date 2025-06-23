#pragma once
#include "integral_constant.hpp"

namespace xstl {

  template<typename T>
  struct has_trivial_destructor : bool_constant<__is_trivially_destructible(T)>{};

  template<typename T>
  constexpr bool has_trivial_destructor_v = has_trivial_destructor<T>::value;
}
