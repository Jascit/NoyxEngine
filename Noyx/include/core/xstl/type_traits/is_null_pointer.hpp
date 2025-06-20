#pragma once
#include "../core/xstldef.hpp"
#include "integral_constant.hpp"
#include "remove_cv.hpp"
#include "is_same.hpp"

namespace xstl {
  template<typename T>
  struct is_null_pointer : is_same<remove_cv_t<T>, nullptr_t> {};

  template<typename T>
  constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
} // xstl
