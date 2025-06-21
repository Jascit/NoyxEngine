#pragma once
#include "is_void.hpp"
#include "is_reference.hpp"
#include "is_function.hpp"

namespace xstl {
  template<typename T>
  struct is_object : bool_constant<!is_void_v<T> && !is_reference_v<T> && !is_function_v<T>> {};
  template<typename T>
  constexpr bool is_object_v = is_object<T>::value;
}
