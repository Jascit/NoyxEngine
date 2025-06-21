#pragma once
#include "integral_constant.hpp"
#include "../core/xstldef.hpp"

namespace xstl {

  template<typename T, xstl::size_t N = 0>
  struct rank : integral_constant<xstl::size_t, N> {};

  template<typename T, xstl::size_t N>
  struct rank<T[], N> : rank<T, N + 1> {};

  template<typename T, xstl::size_t N, xstl::size_t Ns>
  struct rank<T[Ns], N> : rank<T, N + 1> {};

  template<typename T>
  constexpr xstl::size_t rank_v = rank<T>::value;
} // xstl
