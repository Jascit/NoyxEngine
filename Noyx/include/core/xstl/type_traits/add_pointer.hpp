#pragma once
#include "remove_reference.hpp"

namespace xstl {
  namespace details {
    template <typename T>
    struct add_pointer_impl {
      using type = remove_reference_t<T>*;
    };
  }

  template <typename T>
  struct add_pointer {
    using type = T;
  };

  template <typename T>
    requires requires {typename details::add_pointer_impl<T>::type; }
  struct add_pointer<T> : details::add_pointer_impl<T> {};

  template <typename T>
  using add_pointer_t = typename add_pointer<T>::type;

}
