#pragma once
#include <type_traits/integral_constant.hpp>
#include <type_traits/remove_cv.hpp>
namespace xstl {
  namespace details{
    template<typename T>
    struct _is_integral_helper : false_type {};

    template<>
    struct _is_integral_helper<bool > : true_type {};

    template<>
    struct _is_integral_helper<char> : true_type {};

    template<>
    struct _is_integral_helper<signed char> : true_type {};

    template<>
    struct _is_integral_helper<unsigned char> : true_type {};

    template<>
    struct _is_integral_helper<wchar_t> : true_type {};

    template<>
    struct _is_integral_helper<char8_t> : true_type {};

    template<>
    struct _is_integral_helper<char16_t> : true_type {};

    template<>
    struct _is_integral_helper<char32_t> : true_type {};

    template<>
    struct _is_integral_helper<short> : true_type {};

    template<>
    struct _is_integral_helper<unsigned short> : true_type {};

    template<>
    struct _is_integral_helper<int> : true_type {};

    template<>
    struct _is_integral_helper<unsigned int> : true_type {};

    template<>
    struct _is_integral_helper<long> : true_type {};

    template<>
    struct _is_integral_helper<unsigned long> : true_type {};

    template<>
    struct _is_integral_helper<long long> : true_type {};

    template<>
    struct _is_integral_helper<unsigned long long> : true_type {};
  }

  template<typename T>
  struct is_integral : details::_is_integral_helper<remove_cv_t<T>> {};
  
  template<typename T>
  constexpr bool is_integral_v = is_integral<T>::value;

} // xstl