#pragma once 
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_cv.hpp>

namespace xstl {
  namespace details {
    template<typename T>
    struct _is_unsigned_helper {
      constexpr static bool value = (static_cast<T>(-1)) > (static_cast<T>(0));
    };
  } // details

  template<typename T>
  struct is_unsigned : bool_constant<details::_is_unsigned_helper<remove_cv_t<T>>::value> {};

  template<typename T>
  constexpr bool is_unsigned_v = is_unsigned<T>::value;
} // xstl