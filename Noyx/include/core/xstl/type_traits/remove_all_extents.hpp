#pragma once

namespace xstl {
  template<typename T>
  struct remove_all_extents {
    using type = T;
  };

  template<typename T>
  struct remove_all_extents<T[]> {
    using type = remove_all_extents<T>::type;
  };

  template<typename T, size_t Ts>
  struct remove_all_extents<T[Ts]> {
    using type = remove_all_extents<T>::type;
  };

  template<typename T>
  using remove_all_extents_t = remove_all_extents<T>::type;
}
