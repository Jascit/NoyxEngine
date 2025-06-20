#pragma once

namespace xstl {
  template<typename T>
  struct add_const {
    using type = const T;
  };

  template<typename T>
  using add_const_t = add_const<T>::type;
}
