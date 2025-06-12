#pragma once
#include <type_traits/remove_cv.hpp>

namespace xstl {
#if defined(__clang__)
  template <class _Ty>
  constexpr bool is_member_pointer_v = __is_member_pointer(_Ty);
#else
  namespace details {
    template<class T>
    struct _is_member_pointer_helper : false_type {};

    template<class T, class U>
    struct _is_member_pointer_helper<T U::*> : true_type {};
  }

  template<class T>
  struct is_member_pointer : details::_is_member_pointer_helper<xstl::remove_cv_t<T>> {};

  template<class T>
  constexpr bool is_member_pointer_v = is_member_pointer<T>::value;
#endif
}