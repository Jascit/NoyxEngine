#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
  template<typename T>
  struct negation : bool_constant<static_cast<bool>(!T::value)> {};
  template<typename T>
  constexpr bool negation_v = negation<T>::value;
} //xstl