#pragma once
#include <type_traits>

namespace xstl {
  template <typename T>
  struct remove_cv { 
    using type = T;

    template <template <typename> typename Fn>
    using apply = Fn<T>;
  };

  template <typename T>
  struct remove_cv<const T> {
    using type = T;

    template <template <typename> typename Fn>
    using apply = const Fn<T>;
  };

  template <typename T>
  struct remove_cv<volatile T> {
    using type = T;

    template <template <typename> typename Fn>
    using apply = volatile Fn<T>;
  };

  template <typename T>
  struct remove_cv<const volatile T> {
    using type = T;

    template <template <typename> typename Fn>
    using apply = const volatile Fn<T>;
  };

  template <typename T>
  using remove_cv_t = typename remove_cv<T>::type;

  template<typename T, template<typename> typename Base>
  using preserve_cv_t = typename remove_cv<T>::template apply<Base>;
}