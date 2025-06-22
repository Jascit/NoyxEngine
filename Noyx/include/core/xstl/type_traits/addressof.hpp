#pragma once
namespace xstl {
  template<typename T>
  constexpr T* addressof(T& _Val) noexcept {
    return __builtin_addressof(_Val);
  }

  template<typename T>
  T* addressof(T&& _Val) = delete;
} // xstl