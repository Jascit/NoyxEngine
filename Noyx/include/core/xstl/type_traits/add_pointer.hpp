#pragma once
#include <type_traits/remove_reference.hpp>
#include <type_traits/void_t.hpp>

namespace xstl {
  //if T is uncomplete
  template<typename T, typename = void>
  struct add_pointer { 
    using type = T;
  };
  //else, remove_reference
  template<typename T>
  struct add_pointer<T, void_t<remove_reference_t<T>*>> {
    using type = remove_reference_t<T>*;
  };

  template<typename T>
  using add_pointer_t = add_pointer<T>::type;
} // xstl
