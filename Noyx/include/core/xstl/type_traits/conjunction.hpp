#pragma once 
#include <type_traits/integral_constant.hpp>
#include <type_traits/conditional.hpp>

namespace xstl {
  template<typename...>
  struct conjunction : true_type {};

  template<typename T, typename... Args> 
  struct conjunction<T, Args...> : xstl::conditional_t<static_cast<bool>(T::value), conjunction<Args...>, false_type> {};

  template<typename... Args>
  using conjunction_v = xstl::conjunction<Args...>::value;
}