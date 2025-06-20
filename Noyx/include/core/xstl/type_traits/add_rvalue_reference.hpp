#pragma once
#include "void_t.hpp"

namespace xstl{
    namespace details {
        template<typename T>
        concept _ConceptRValueReferenceable = requires(T && t) {
            (void)t;
        };
    }
  template<typename T, typename = void>
  struct add_rvalue_reference {
    using type = T;
  };
  template<details::_ConceptRValueReferenceable T>
  struct add_rvalue_reference<T, xstl::void_t<T&&>> 
  {
    using type = T&&;
  };
  template<typename T>
  using add_rvalue_reference_t = add_rvalue_reference<T>::type;
} // xstl 
