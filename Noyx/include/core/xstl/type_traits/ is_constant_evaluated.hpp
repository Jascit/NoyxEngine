#pragma once

namespace xstl {

  constexpr bool is_constant_evaluated() noexcept {
#if defined(__clang__) || defined(__GNUC__)
    return __builtin_is_constant_evaluated();

#elif defined(_MSC_VER)
    return __is_constant_evaluated();
#endif
  }
}
