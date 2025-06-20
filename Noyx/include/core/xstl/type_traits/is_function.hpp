#pragma once
#include "remove_cvref_t.hpp"
#include "is_const.hpp"
#include "is_reference.hpp"
#include "integral_constant.hpp"

namespace xstl {
  template<class T>
  struct is_function : bool_constant<!is_const<const T>::value && !is_reference<T>::value> {};
  template<typename T>
  constexpr bool is_function_v = is_function<T>::value;

} // xstl
