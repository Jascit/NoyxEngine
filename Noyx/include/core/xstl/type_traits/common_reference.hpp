#pragma once
#include "common_type.hpp"
#include "remove_reference.hpp"
//TODO: don't work, need to find the error
namespace xstl {
  namespace details {
    template<typename T, typename U>
    struct _common_reference_helper {};

    template<typename T, typename U>
    struct _common_reference_helper<T&, U&> {
      using type = common_type_t<remove_reference_t<T>, remove_reference_t<U>>&;
    };

    template<typename T, typename U>
    struct _common_reference_helper<T&&, U&&> {
      using type = common_type_t<remove_reference_t<T>, remove_reference_t<U>>&&;
    };

    template<typename T, typename U>
    struct _common_reference_helper<T&, U&&> {
      using type = common_type_t<remove_reference_t<T>, remove_reference_t<U>>&;
    };

    template<typename T, typename U>
    struct _common_reference_helper<T&&, U&> {
      using type = common_type_t<remove_reference_t<T>, remove_reference_t<U>>&;
    };

    template<typename T, typename U>
    concept _ConceptHasCommonReference = requires {
      typename details::_common_reference_helper<T, U>::type;
    };
  }
  template<typename...>
  struct common_reference {};

  template<typename T>
  struct common_reference<T, T> {
    using type = T;
  };

  template<>
  struct common_reference<> {};

  template<typename T, typename U, typename... Args>
  requires details::_ConceptHasCommonReference<T, U> 
  struct common_reference<T, U, Args...> : common_reference<typename details::_common_reference_helper<T, U>::type, Args...> {};

  template<typename T, typename U>
  requires details::_ConceptHasCommonReference<T, U>
  struct common_reference<T, U> : details::_common_reference_helper<T, U>{};

  template<typename... Args>
  using common_reference_t = typename common_reference<Args...>::type;
}

