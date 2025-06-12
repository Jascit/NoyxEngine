#pragma once
#include <type_traits/is_same.hpp>
#include <type_traits>
std::is_null_pointer<int>;
namespace xstl{
  template<typename T>
  struct is_void : is_same<T, void>{};
  template<typename T>
  constexpr bool is_void_v = is_void<T>::value;
} // xstl