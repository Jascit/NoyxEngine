#pragma once

namespace xstl {
  template<typename T>
  struct remove_extent {
    using type = T;
  };
  
  template<typename T>
  struct remove_extent<T[]> {
    using type = T;
  };

  template<typename T, size_t Ts>
  struct remove_extent<T[Ts]> {
    using type = T;
  };

  template<typename T>
  using remove_extent_t = remove_extent<T>::type;
}
