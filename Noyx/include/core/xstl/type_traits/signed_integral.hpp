#pragma once
#include <type_traits/is_signed.hpp>
#include <type_traits/is_integral.hpp>

namespace xstl {
  template<typename T>
  concept signed_integral = is_integral_v<T> && is_signed_v<T>;
}