#pragma once 
#include "integral_constant.hpp"
#include "conditional.hpp"
namespace xstl {
  template<typename...>
  struct disjunction : false_type {};

  template<typename T, typename... Args>
  struct disjunction<T, Args...> : xstl::conditional_t<static_cast<bool>(T::value), true_type, disjunction<Args...>> {};
  
  template<typename... Args>
  constexpr bool disjunction_v = disjunction<Args...>::value;
} 
