#pragma once
#include "remove_reference.hpp"
#include "is_lvalue_reference.hpp"

namespace xstl {
  template<typename T>
  constexpr T&& forward(remove_reference_t<T>& obj) noexcept {
    static_cast<T&&>(obj);
  }

  template<typename T>
  constexpr T&& forward(remove_reference_t<T>&& obj) noexcept {
    static_assert(!is_lvalue_reference_v<T>, "Cannot forward an rvalue as an lvalue");
    return static_cast<T&&>(obj);
  }
} // xstl
