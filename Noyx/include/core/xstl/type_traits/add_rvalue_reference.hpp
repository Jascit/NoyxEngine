#pragma once
#include <void_t.hpp>

namespace xstl{
  template<typename T, typename = void>
  struct add_rvalue_reference {
    using type = T;
  };
  template<typename T>
  struct add_rvalue_reference<T, xstl::void_t<T&&>>
  {
    using type = T&&;
  };
  template<typename T>
  using add_rvalue_reference_t = add_rvalue_reference<T>::type;
} // xstl