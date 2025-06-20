#pragma once
#include "../core/xstldef.hpp"
namespace xstl {
  template <typename T>
  struct remove_cv { 
    using type = T;
  };

  template <typename T>
  struct remove_cv<const T> {
    using type = T;
  };
  template <typename T>
  struct remove_cv<const T[]> {
    using type = T[];
  };
  template <typename T, xstl::size_t N>
  struct remove_cv<const T[N]> {
    using type = T[N];
  };

  template <typename T>
  struct remove_cv<volatile T> {
    using type = T;
  };
  template <typename T>
  struct remove_cv<volatile T[]> {
    using type = T[];
  };
  template <typename T, xstl::size_t N>
  struct remove_cv<volatile T[N]> {
    using type = T[N];
  };

  template <typename T>
  struct remove_cv<const volatile T> {
    using type = T;
  };

  template <typename T>
  struct remove_cv<const volatile T[]> {
    using type = T[];
  };

  template <typename T, xstl::size_t N>
  struct remove_cv<const volatile T[N]> {
    using type = T[N];
  };

  template <typename T>
  using remove_cv_t = typename remove_cv<T>::type;
}
