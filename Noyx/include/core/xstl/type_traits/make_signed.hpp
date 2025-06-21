#pragma once
#include "../core/xstldef.hpp"
#include "remove_cv.hpp"
#include "is_same.hpp"
#include "is_integral.hpp"

//only LLP64/LP64
namespace xstl {
  namespace details {
    template<xstl::size_t Ts>
    struct _make_signed_helper; // never called

    template<>
    struct _make_signed_helper<1> {
      using type = signed char;
    };

    template<>
    struct _make_signed_helper<2> {
      using type = short;
    };

    template<>
    struct _make_signed_helper<4> {
      using type = int;
    };

    template<>
    struct _make_signed_helper<8> {
      using type = long long;
    };
  }

  template<typename T>
  struct make_signed {
    static_assert(!xstl::is_same_v<xstl::remove_cv_t<T>, bool>, "make_signed does not support bool");
    static_assert(xstl::is_integral_v<T>, "make_signed supports only integrals");
    using type = typename details::_make_signed_helper<sizeof(T)>::type;
  };

  
  template<typename T>
  using make_signed_t = make_signed<T>::type;
} // xstl
