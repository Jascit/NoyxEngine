#pragma once

#include <type_traits/integral_constant.hpp>

namespace xstl {

  template <typename T>
  struct is_signed : xstl::false_type {};

  template <>
  struct is_signed<signed char> : xstl::true_type {};

  template <>
  struct is_signed<short> : xstl::true_type {};

  template <>
  struct is_signed<int> : xstl::true_type {};

  template <>
  struct is_signed<long> : xstl::true_type {};

  template <>
  struct is_signed<long long> : xstl::true_type {};

  template <typename T>
  constexpr bool is_signed_v = is_signed<T>::value;

}
