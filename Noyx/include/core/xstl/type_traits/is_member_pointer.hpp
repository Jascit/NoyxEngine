#pragma once
#include <type_traits/remove_cv.hpp>

namespace xstl {
#if defined(__clang__)
  template <typename _Ty>
  constexpr bool is_member_pointer_v = __is_member_pointer(_Ty);
#else
  namespace details {
    template<typename T>
    struct _is_member_pointer_helper : false_type {};

    template<typename T, typename Tc>
    struct _is_member_pointer_helper<T Tc::*> : true_type {};
  }

  template<typename T>
  struct is_member_pointer : details::_is_member_pointer_helper<xstl::remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
#endif
}