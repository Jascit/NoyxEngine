#pragma once 
#include "integral_constant.hpp"
//TODO: Clang/GCC 
namespace xstl {
  template<typename T>
  struct is_class : bool_constant<__is_class(T)>{};
  
  template<typename T>
  constexpr bool is_class_v = is_class<T>::value;
}
