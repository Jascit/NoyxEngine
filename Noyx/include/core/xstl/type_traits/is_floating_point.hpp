#pragma once
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_cv.hpp>
namespace xstl {
  namespace details {
    template<typename T>
    struct _is_floating_point_helper : false_type {};

    template<>
    struct _is_floating_point_helper<float> : true_type {};
    
      template<>
    struct _is_floating_point_helper<double> : true_type {};

    template<>
    struct _is_floating_point_helper<long double> : true_type {};
  }

  template<typename T>
  struct is_floating_point : details::_is_floating_point_helper<remove_cv_t<T>> {};

  template<typename T>
  constexpr bool is_floating_point_v = is_floating_point<T>::value;

} // xstl