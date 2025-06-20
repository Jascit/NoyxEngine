#pragma once 
#include <type_traits/is_function.hpp>

namespace xstl {
#if defined(__clang__)
  template <class T>
  struct is_member_object_pointer : bool_constant<__is_member_object_pointer(T)> {};

  template <class T>
  constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
#else
  namespace details {
    template<typename>
    struct _is_member_object_pointer_helper : false_type{};
    
    template<typename T, typename Tc>
    struct _is_member_object_pointer_helper<T Tc::*> : bool_constant<!is_function_v<T>> {};
  }
  
  template<typename T>
  struct is_member_object_pointer : details::_is_member_object_pointer_helper<remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;
#endif
} // xstl