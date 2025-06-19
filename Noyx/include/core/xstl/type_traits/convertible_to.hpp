#pragma once
#include <type_traits/is_convertible.hpp>
#include <type_traits/declval.hpp>

namespace xstl {
  template<typename T, typename U>
  concept convertible_to = is_convertible_v<T, U> && requires { static_cast<U>(declval<T>()); };
}