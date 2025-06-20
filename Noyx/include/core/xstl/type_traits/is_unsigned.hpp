#pragma once

#include <type_traits/integral_constant.hpp>

namespace xstl {

  template <typename T>
  struct is_unsigned : xstl::false_type {};

  template <>
  struct is_unsigned<unsigned char> : xstl::true_type {};

  template <>
  struct is_unsigned<unsigned short> : xstl::true_type {};

  template <>
  struct is_unsigned<unsigned int> : xstl::true_type {};

  template <>
  struct is_unsigned<unsigned long> : xstl::true_type {};

  template <>
  struct is_unsigned<unsigned long long> : xstl::true_type {};

  template <typename T>
  constexpr bool is_unsigned_v = is_unsigned<T>::value;

}
