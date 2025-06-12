#pragma once 
#include <type_traits/is_function.hpp>

namespace xstl {
  namespace details {
    template<class T>
    struct _is_member_function_pointer_helper : false_type {};

    template<class T, class U>
    struct _is_member_function_pointer_helper<T U::*> : is_function<T> {};
  }

  template<class T>
  struct is_member_function_pointer : details::_is_member_function_pointer_helper<xstl::remove_cv_t<T>> {};

  template<class T>
  constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;
} // xstl