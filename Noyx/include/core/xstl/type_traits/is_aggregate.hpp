#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct is_aggregate : bool_constant<__is_aggregate(T)>{};
  
  template<typename T>
  constexpr bool is_aggregate_v = is_aggregate<T>::value;
}
