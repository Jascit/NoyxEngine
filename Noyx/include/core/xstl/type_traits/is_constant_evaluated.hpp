#pragma once

namespace xstl {
  constexpr bool is_constant_evaluated() noexcept {
    return __builtin_is_constant_evaluated();
  }
}
