#pragma once
#include <type_traits/convertible_to.hpp>
#include <type_traits/remove_reference.hpp>

namespace xstl {
  namespace details {
    template<typename T>
    concept _boolean_testable_helper = convertible_to<T, bool>;

    template<typename T>
    concept boolean_testable = _boolean_testable_helper<T> && requires(T && t) {
      { !static_cast<T&&>(t) } -> _boolean_testable_helper;
    };

    template<typename T, typename U>
    concept _half_equality_comparable = requires(const remove_reference_t<T>&t, const remove_reference_t<U>&u) {
      { t == u } -> boolean_testable;
      { t != u } -> boolean_testable;
    };
  }

  template<typename T>
  concept equality_comparable = details::_half_equality_comparable<T, T>; 
}
