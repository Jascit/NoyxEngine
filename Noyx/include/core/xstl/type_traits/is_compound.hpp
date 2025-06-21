#pragma once
#include "is_fundamental.hpp"

namespace xstl {
  template<typename T>
	struct is_compound : bool_constant<!is_fundamental_v<remove_cv_t<T>>> {};
  template<typename T>
  constexpr bool is_compound_v = is_compound<T>::value;
} // xstl
