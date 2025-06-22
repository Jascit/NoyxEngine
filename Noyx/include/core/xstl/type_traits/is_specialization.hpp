#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<template<typename...> typename base, typename spec>
  struct is_specialization : false_type {};
  
  template<template<typename...> typename base, typename... Args>
  struct is_specialization<base, base<Args...>> : true_type {};

  template<template<typename...> typename base, typename spec>
  constexpr bool is_specialization_v = is_specialization<base, spec>::value;
} // xstl