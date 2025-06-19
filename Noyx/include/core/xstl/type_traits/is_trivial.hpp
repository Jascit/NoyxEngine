#pragma once
#include <type_traits>
#include <type_traits/integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct is_trivial : bool_constant<__is_trivial(T)>{};

  template<typename T>
  constexpr bool is_trivial_v = is_trivial<T>::value;
}
