#pragma once
#include "is_convertible.hpp"
#include "declval.hpp"

namespace xstl {
  template<typename T, typename U>
  concept convertible_to = is_convertible_v<T, U> && requires { static_cast<U>(declval<T>()); };
}
