#pragma once

namespace xstl {
  template<typename T>
  struct add_cv {
    using type = const volatile T;
  };

  template<typename T>
  using add_cv_t = typename add_cv<T>::type;
} // xstl
