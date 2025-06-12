#pragma once
#include <type_traits/integral_constant.hpp>

namespace xstl {
  
  template<typename T, typename U>
  struct is_same : public xstl::false_type {};

  template<typename T>
  struct is_same<T, T> : public xstl::true_type{};

  template<typename T, typename U>
  constexpr bool is_same_v = is_same<T, U>::value;
}
