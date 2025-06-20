#pragma once
#include "integral_constant.hpp"
#include "../core/xstldef.hpp"

namespace xstl {
  template<typename T, xstl::size_t N = 0 >
  struct extent : integral_constant<xstl::size_t, 0> {};

  template<typename T, xstl::size_t M>
  struct extent<T[M], 0> : integral_constant<xstl::size_t, M> {};

  template<typename T, xstl::size_t M, xstl::size_t N >
  struct extent<T[M], N> : extent<T, N - 1> {};

  template<typename T, xstl::size_t N = 0>
  constexpr xstl::size_t extent_v = extent<T, N>::value;
} //  xstl
