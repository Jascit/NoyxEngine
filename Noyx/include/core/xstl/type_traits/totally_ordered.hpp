#pragma once
#include <type_traits/remove_reference.hpp>
#include <type_traits/equality_comparable.hpp>

namespace xstl {
  namespace details {
    template<typename T, typename U>
    concept _half_ordered_helper = requires(const remove_reference_t<T>&t, const remove_reference_t<U>&u) {
      { t < u } -> boolean_testable;
      { t > u } -> boolean_testable;
      { t == u } -> boolean_testable;
      { t != u } -> boolean_testable;
    };
  }

  template<typename T>
  concept totally_ordered = equality_comparable<T> && details::_half_ordered_helper<T, T>;
}
