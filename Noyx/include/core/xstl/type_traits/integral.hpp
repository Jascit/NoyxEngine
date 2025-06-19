#pragma once
#include <type_traits/is_integral.hpp>

namespace xstl {
  template<typename T>
  concept integral = is_integral_v<T>;
}