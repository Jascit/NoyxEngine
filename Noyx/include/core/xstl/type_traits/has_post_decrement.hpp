#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_post_decrement : xstl::false_type{};

  template<typename T>
    requires requires (T& t) { t--; }
  struct has_post_decrement<T> : xstl::true_type{};
}