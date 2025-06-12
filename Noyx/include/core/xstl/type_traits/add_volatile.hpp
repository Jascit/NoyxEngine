#pragma once

namespace xstl {
  template<typename T>
  struct add_volatile {
    using type = volatile T;
  };

  template <class _Ty>
  using add_volatile_t = typename add_volatile<_Ty>::type;
} // xstl