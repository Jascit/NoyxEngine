#pragma once
#include <type_traits/remove_cvref_t.hpp>
#include <type_traits/integral_constant.hpp>
//TODO: don't work
namespace xstl {
  template<typename T>
  struct _Is_function_helper : false_type {};

  template<typename R, typename... Args>
  struct _Is_function_helper<R(Args...)> : true_type {};

  template<typename R, typename... Args>
  struct _Is_function_helper<R(Args..., ...)> : true_type {};

  template<typename R, typename... Args>
  struct _Is_function_helper<R(Args...) noexcept> : true_type {};

  template<typename R, typename... Args>
  struct _Is_function_helper<R(Args..., ...) noexcept> : true_type {};

  template<typename T>
  struct is_function : _Is_function_helper<xstl::remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_function_v = is_function<T>::value;

} // xstl