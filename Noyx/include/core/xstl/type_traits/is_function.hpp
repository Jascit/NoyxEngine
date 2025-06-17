#pragma once
#include <type_traits/remove_cvref_t.hpp>
#include <type_traits/integral_constant.hpp>

namespace xstl {
  namespace details {
    template<typename T>
    struct _is_function_helper : false_type {};

    template<typename Func, typename... Args>
    struct _is_function_helper<Func(Args...)> : true_type {};

    template<typename Func, typename... Args>
    struct _is_function_helper<Func(Args..., ...)> : true_type {};

    template<typename Func, typename... Args>
    struct _is_function_helper<Func(Args...) noexcept> : true_type {};

    template<typename Func, typename... Args>
    struct _is_function_helper<Func(Args..., ...) noexcept> : true_type {};
  }
  template<typename T>
  struct is_function : details::_is_function_helper<xstl::remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_function_v = is_function<T>::value;

} // xstl