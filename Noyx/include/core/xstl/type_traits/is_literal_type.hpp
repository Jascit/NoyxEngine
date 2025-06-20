#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct is_literal_type : bool_constant<__is_literal_type(T)>{};
  
  template<typename T>
  constexpr bool is_literal_type_v = is_literal_type<T>::value;
}
