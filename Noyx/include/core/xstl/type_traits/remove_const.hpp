#pragma once
#include <type_traits>
std::remove_const<int>;
namespace xstl {
  template<typename T>
  struct remove_const {
    using type = T;
  };

  template<typename T>
  struct remove_const<const T> {
    using type = T;
  };

  template<typename T, std::size_t N>
  struct remove_const<const T[N]> {
    using type = T[N];
  };

  template<typename T>
  struct remove_const<const T[]> {
    using type = T[];
  };

  template <typename T>
  using remove_const_t = typename remove_const<T>::type;

} // namespace xstl
