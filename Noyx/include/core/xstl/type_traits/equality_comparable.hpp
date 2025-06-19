#pragma once
#include <type_traits/convertible_to.hpp>

namespace xstl {
  namespace details {
    template<typename T>
    concept _boolean_testable = convertible_to<T, bool>;

    template<typename T>
    concept boolean_testable = _boolean_testable<T> && requires(T && t) {
      { !static_cast<T&&>(t) } -> _boolean_testable;
    };
  }

}
