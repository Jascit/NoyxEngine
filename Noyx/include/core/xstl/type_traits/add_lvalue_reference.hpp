#pragma once
#include <type_traits/void_t.hpp>

namespace xstl {
    namespace details {
        template<typename T>
        concept _ConceptLValueReferenceable = requires(T && t) {
            (void)t;
        };
    }
  template<typename T>
  struct add_lvalue_reference {
    using type = T;
  };
  template<details::_ConceptLValueReferenceable T>
  struct add_lvalue_reference<T>
  {
    using type = T&;
  };
  template<typename T>
  using add_lvalue_reference_t = add_lvalue_reference<T>::type;
} // xstl