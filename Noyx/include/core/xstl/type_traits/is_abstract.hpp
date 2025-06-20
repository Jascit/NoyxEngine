#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct is_abstract : bool_constant<__is_abstract(T)> {};

  template<typename T>
  constexpr bool is_abstract_v = is_abstract<T>::value;
}