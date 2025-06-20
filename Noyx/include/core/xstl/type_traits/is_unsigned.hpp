#pragma once
#include <type_traits/is_integral.hpp>
#include <type_traits/is_floating_point.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_cv.hpp>

namespace xstl {
  namespace details {
    template<typename T, bool = is_integral_v<T>>
    struct _is_unsigned_helper {
      constexpr static bool value = (static_cast<T>(-1)) > (static_cast<T>(0));
    };
    template<typename T>
    struct _is_unsigned_helper<T, false> {
      constexpr static bool value = false;
    };
  } // details

  template<typename T>
  struct is_unsigned : bool_constant<details::_is_unsigned_helper<remove_cv_t<T>>::value> {};

  template<>
  struct is_unsigned<bool> {
    constexpr static bool value = false;
  };

  template<typename T>
  constexpr bool is_unsigned_v = is_unsigned<T>::value;
} // xstl
