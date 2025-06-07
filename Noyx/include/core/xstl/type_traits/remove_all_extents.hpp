#pragma once

namespace xstl {
  template<typename T>
  struct remove_all_exstens {
    using type = T;
  };

  template<typename T>
  struct remove_all_exstens<T[]> {
    using type = remove_all_exstens<T>::type;
  };

  template<typename T, size_t Ts>
  struct remove_all_exstens<T[Ts]> {
    using type = remove_all_exstens<T>::type;
  };

  template<typename T>
  using remove_all_exstens_t = remove_all_exstens<T>::type;
  
}