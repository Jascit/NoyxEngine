#pragma once
#include "../core/xstldef.hpp"
#include "remove_cv.hpp"
#include "is_same.hpp"
#include "is_integral.hpp"

//only LLP64/LP64
namespace xstl {
  namespace details {
    template <xstl::size_t Ts>
    struct _make_unsigned_helper; // undefined

    template <>
    struct _make_unsigned_helper<1> {
      using type = unsigned char;
    };

    template <>
    struct _make_unsigned_helper<2> {
      using type = unsigned short;
    };

    // Для 4 байт можна додати перевірку типів
    template <>
    struct _make_unsigned_helper<4> {
      using type = unsigned int;  // або unsigned long, якщо це твоя платформа
    };

    template <>
    struct _make_unsigned_helper<8> {
      using type = unsigned long long;
    };
  }

  template<typename T>
  struct make_unsigned {
    static_assert(!xstl::is_same_v<xstl::remove_cv_t<T>, bool>, "make_unsigned does not support bool");
    static_assert(xstl::is_integral_v<T>, "make_unsigned supports only integrals");
    using type = typename details::_make_unsigned_helper<sizeof(T)>::type;
  };


  template<typename T>
  using make_unsigned_t = make_unsigned<T>::type;
} // xstl
