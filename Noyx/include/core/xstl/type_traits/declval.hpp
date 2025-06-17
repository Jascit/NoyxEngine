#pragma once
#include <type_traits/add_rvalue_reference.hpp>

namespace xstl {
  template<typename T>
  constexpr add_rvalue_reference_t<T> declval() noexcept {
    static_assert(false, "declval() must not be called at runtime");
  };
} // xstl