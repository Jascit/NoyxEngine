#pragma once
#include <type_traits/is_base_of.hpp>
#include <type_traits/is_convertible.hpp>

namespace xstl {
  template<typename T, typename U>
  concept derived_from = is_base_of_v<T, U> && is_convertible_v<const volatile T*, const volatile U*>;
}
