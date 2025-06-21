#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_virtual_destructor : bool_constant<__has_virtual_destructor(T)>{};
   			
  template<typename T>
  constexpr bool has_virtual_destructor_v = has_virtual_destructor<T>::value;
}	
