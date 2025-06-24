#pragma once
#include "integral_constant.hpp"
namespace xstl {
  template<typename T>
  struct has_pre_increment : xstl::false_type {};

  template<typename T>
    requires requires (T& t) { ++t; }
  struct has_pre_increment<T> : xstl::true_type {};
}