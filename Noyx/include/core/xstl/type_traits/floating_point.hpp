#pragma once
#include <type_traits/is_floating_point.hpp>

namespace xstl {
  template<typename T>
  concept floating_point = is_floating_point_v<T>;
}