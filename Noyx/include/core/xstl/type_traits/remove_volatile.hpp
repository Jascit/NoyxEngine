#pragma once
#include "../core/xstldef.hpp"
namespace xstl {
  template <typename T>
  struct remove_volatile {
    using type = T;
  };
  
  template <typename T>
  struct remove_volatile<volatile T> {
    using type = T;
  };

  template <typename T> 
  struct remove_volatile<volatile T[]> {
    using type = T[]; 
  };
  
  template<typename T, xstl::size_t N >
  struct remove_volatile<volatile T[N]> {
    using type = T[N];
  };

  template <typename T>
  using remove_volatile_t = typename remove_volatile<T>::type;

} // xstl
