#pragma once
#include "integral_constant.hpp"
#include "is_arithmetic.hpp"
#include "is_null_pointer.hpp"
#include "is_void.hpp"

namespace xstl {
  namespace details {
    template<typename T>
    struct _is_fundamental_helper : bool_constant<is_arithmetic_v<T> || is_null_pointer_v<T> || is_void_v<T>> {};
  }
  template<typename T>
  struct is_fundamental : details::_is_fundamental_helper<remove_cv_t<T>> {};
  
  template<typename T>
  constexpr bool is_fundamental_v = is_fundamental<T>::value;
} // xstl
