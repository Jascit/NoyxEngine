#pragma once
#include "integral_constant.hpp"

namespace xstl {
  namespace details {
    template<typename T>
    concept _ConceptIsComplete = requires { sizeof(T); };
  }

  template <typename T, typename = void>
  struct is_complete : false_type {};

  template <typename T>
  requires details::_ConceptIsComplete<T>
  struct is_complete<T, void> : true_type{};

  template <typename T>
  constexpr bool is_complete_v = is_complete<T>::value;
}
