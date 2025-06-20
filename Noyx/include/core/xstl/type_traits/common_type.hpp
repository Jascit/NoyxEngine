#pragma once 
#include <type_traits/declval.hpp>
#include <type_traits/decay.hpp>

namespace xstl {
  namespace details {
    template<typename T, typename U>
    using _common_type_finder = decltype(false ? xstl::declval<T>() : xstl::declval<U>());

    template<typename T, typename U>
    concept _ConceptCommonTypeExists = requires {
      typename _common_type_finder<T, U>; 
    };

    template<typename T, typename U, typename = void>
    struct _decay_common_type_finder {};

    template<typename T, typename U>
    requires _ConceptCommonTypeExists<T, U>
    struct _decay_common_type_finder<T, U, void> {
      using type = decay_t<_common_type_finder<T, U>>;
    };

    template<typename T, typename U, typename = void>
    struct _common_type_impl : _decay_common_type_finder<const T&, const U&> {};

    template<typename T, typename U>
    requires _ConceptCommonTypeExists<T, U>
    struct _common_type_impl<T, U, void> : _decay_common_type_finder<T, U> {};

    template<typename T, typename U, typename Td = decay_t<T>, typename Ud = decay_t<U>>
    struct _common_type_helper : _common_type_impl<Td, Ud> {};

    template<typename T, typename U>
    struct _common_type_helper<T, U, T, U> : _common_type_impl<T, U> {};

    template<typename... Args>
    struct _common_type_args_helper {};

    template<typename T, typename U>
    requires requires{ typename _common_type_helper<T, U>::type; }
    struct _common_type_args_helper<T, U> : _common_type_helper<T, U> {};

    template<typename T, typename U, typename... Args>
    requires requires{ typename _common_type_helper<T, U>::type; }
    struct _common_type_args_helper<T, U, Args...> : _common_type_args_helper<typename _common_type_helper<T, U>::type, Args...> {};
  }

  template<typename... Args>
  struct common_type;
  
  template<typename... Args>
  using common_type_t = common_type<Args...>::type;
  
  template<>
  struct common_type<> {};
  
  template<typename T>
  struct common_type<T> : common_type<T, T> {};

  template<typename T, typename U>
  struct common_type<T, U> : details::_common_type_helper<T, U> {};
  
  template<typename T, typename U, typename... Args>
  struct common_type<T, U, Args...> : details::_common_type_args_helper<T, U, Args...> {};
} // xstl


