#pragma once
#include <type_traits/void_t.hpp>
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_reference.hpp>
#include <type_traits/declval.hpp>
#include <type_traits>
int a() {
  int a;
  int b;
  std::swap(a, b);
}
//GCC, Clang
namespace xstl {
#if defined(_MSVC_LANG)
  template<typename T, typename = void>
  struct is_move_constructible : bool_constant<__is_constructible(T, remove_reference_t<T>&&)>{};

  _EXPORT_STD template <class T>
  constexpr bool is_move_constructible_v = is_move_constructible<T>::value;

  _EXPORT_STD template <class T>
  struct is_nothrow_move_constructible : bool_constant<__is_nothrow_constructible(T, remove_reference_t<T>&&)> {};

  _EXPORT_STD template <class T>
  constexpr bool is_nothrow_move_constructible_v = is_nothrow_move_constructible<T>::value;

#else 
  template<typename T, typename = void>
  struct is_move_constructible : false_type {};

  template<typename T>
  struct is_move_constructible<T, void_t<decltype(T(declval<T&&>()))>> : true_type {};

  template<typename T, typename = void>
  struct is_nothrow_move_constructible : false_type {};

  template<typename T>
  struct is_nothrow_move_constructible<T, void_t<decltype(T(declval<T&&>()))>> : bool_constant<noexcept(T(std::declval<T&&>()))>{};
#endif // _MSVC_VER 
}