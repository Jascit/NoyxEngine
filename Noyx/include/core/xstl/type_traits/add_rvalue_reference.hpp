#pragma once
#include "../core/xstldef.hpp"

namespace xstl{
    namespace details {
        template<typename T>
        concept _ConceptRValueReferenceable = requires(T&& t) {
            (void)t;
        };
    }
  template<typename T>
  struct add_rvalue_reference {
    using type = T;
  };

  template<typename T>
  requires details::_ConceptRValueReferenceable<T>
  struct add_rvalue_reference<T> 
  {
    using type = T&&;
  };
  
  template<typename T>
  using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;
} // xstl 
