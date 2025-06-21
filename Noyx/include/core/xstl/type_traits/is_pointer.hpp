#pragma once
#include "integral_constant.hpp"
#include "remove_cv.hpp"
namespace xstl {
  namespace details {
    template<typename>
    struct _is_pointer_helper : false_type{};

    template<typename T>
    struct _is_pointer_helper<T*> : true_type{};
    
    template<typename T>
    struct _is_pointer_helper<T* const> : true_type{};
    
    template<typename T>
    struct _is_pointer_helper<T* volatile> : true_type{};
    
    template<typename T>
    struct _is_pointer_helper<T* const volatile> : true_type{};
  }
  template<typename T>
  struct is_pointer : details::_is_pointer_helper<remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_pointer_v = is_pointer<T>::value;
} // xstl
