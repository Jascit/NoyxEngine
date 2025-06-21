#pragma once

namespace xstl {
  template<typename T, T Val>
  struct integral_constant {
    static constexpr T value = Val;
    using value_type = T;
    using type = integral_constant;

    [[nodiscard]] constexpr T operator()() const noexcept { return value;}
    constexpr operator T() const noexcept { return value; }
  };

  template<bool Val>
  using bool_constant = integral_constant<bool, Val>;

  using true_type = bool_constant<true>;
  using false_type = bool_constant<false>;
}
