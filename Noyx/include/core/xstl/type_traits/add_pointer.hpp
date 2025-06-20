#pragma once
#include "is_reference.hpp"
#include "void_t.hpp"

namespace xstl {
  template <typename T, bool = xstl::is_reference_v<T>>
  struct add_pointer_impl {
    using type = T*;
  };

  template <typename T>
  struct add_pointer_impl<T, true> {
    using type = T;
  };

  template <typename T>
  struct add_pointer {
    using type = typename add_pointer_impl<T>::type;
  };

  template <typename T>
  using add_pointer_t = typename add_pointer<T>::type;

}
