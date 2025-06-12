#pragma once
#include <type_traits/void_t.hpp>

namespace xstl {
  template<typename T, typename = void>
  struct add_lvalue_reference {
    using type = T;
  };
  template<typename T>
  struct add_lvalue_reference<T, xstl::void_t<T&>>
  {
    using type = T&;
  };
  template<typename T>
  using add_lvalue_reference_t = add_lvalue_reference<T>::type;
} // xstl