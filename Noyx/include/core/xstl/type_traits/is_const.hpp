#pragma once
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_volatile.hpp>
namespace xstl {
  namespace details {
    template<typename>
    struct _is_const_helper : false_type {};
    template<typename T>
    struct _is_const_helper<const T> : true_type {};
  }
  template<typename T>
	struct is_const : details::_is_const_helper<remove_volatile_t<T>>{};
  template<typename T>
	constexpr bool is_const_v = is_const<T>::value;
} // xstl