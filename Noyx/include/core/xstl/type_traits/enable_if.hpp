#pragma once
#include<type_traits/void_t.hpp>
namespace xstl {
  template<bool cond, typename T = void>
  struct enable_if{};

  template<typename T>
  struct enable_if<true, T> {
    using type = T;
  };

  template<bool cond, typename T>
  using enable_if_t = enable_if<cond, T>::type;
} //xstl