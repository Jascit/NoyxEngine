#pragma once
#include "remove_reference.hpp"

namespace xstl {
  template<typename T>
  constexpr remove_reference_t<T>&& move(T&& obj) noexcept {
    return static_cast<remove_reference_t<T>&&>(obj);
  }
} // xstl
