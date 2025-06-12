#pragma once
#include <core/xstldef.hpp>
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_cv.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits>

namespace xstl {
  template<typename T>
  struct is_null_pointer : bool_constant<is_same_v<remove_cv_t<T>, nullptr_t>> {};

  template<typename T>
  constexpr bool is_null_pointer_v = is_null_pointer<T>::value;
} // xstl