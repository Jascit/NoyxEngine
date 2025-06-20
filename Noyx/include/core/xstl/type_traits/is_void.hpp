#pragma once
#include "is_same.hpp"
#include "remove_cv.hpp"

namespace xstl{
  template<typename T>
  struct is_void : is_same<remove_cv_t<T>, void>{};

  template<typename T>
  constexpr bool is_void_v = is_void<T>::value;
} // xstl
