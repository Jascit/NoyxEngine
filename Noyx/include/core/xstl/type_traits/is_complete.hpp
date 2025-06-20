#pragma once
#include "../type_traits/integral_constant.hpp"

namespace xstl {
  namespace details {
    template<typename T>
    concept _ConceptIsComplete = requires { sizeof(T); };
  }

  template <typename T, typename = void>
  struct is_complete : xstl::false_type {};

  template <typename T>
  requires details::_ConceptIsComplete<T>
  struct is_complete<T, void> : xstl::true_type{};

}