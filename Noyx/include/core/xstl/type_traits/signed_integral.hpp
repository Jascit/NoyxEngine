#pragma once
#include "is_signed.hpp"
#include "is_integral.hpp"

namespace xstl {
  template<typename T>
  concept signed_integral = is_integral_v<T> && is_signed_v<T>;
}
