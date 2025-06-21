#pragma once
#include<type_traits/is_arithmetic.hpp>
#include<type_traits/is_pointer.hpp>
#include<type_traits/is_member_pointer.hpp>
#include<type_traits/is_enum.hpp>
#include<type_traits/is_null_pointer.hpp>

namespace xstl {
  template<typename T>
  struct is_scalar : bool_constant<is_arithmetic_v<T> || is_pointer_v<T> || is_enum_v<T> || is_member_pointer_v<T> || is_null_pointer_v<T>>{};
  template<typename T>
  constexpr bool is_scalar_v = is_scalar<T>::value;
} // xstl
