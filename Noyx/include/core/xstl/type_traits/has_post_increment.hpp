#pragma once
#include "integral_constant.hpp"

namespace xstl {
  template<typename T>
  struct has_post_increment : false_type{};

  template<typename T>
    requires requires { xstl::declval<T>()++; }
  struct has_post_increment<T> : true_type{};
}