#pragma once
#include <type_traits>
#include <tuple>

namespace xstl_test_detail {
  // 1) identity
  template<typename T> using suffix_identity = T;

  // 2) const
  template<typename T> using suffix_const = T const;

  // 3) volatile
  template<typename T> using suffix_volatile = T volatile;

  // 4) const volatile
  template<typename T> using suffix_cv = T const volatile;

  // 5) T*
  template<typename T> using suffix_ptr = T*;

  // 6) T* const
  template<typename T> using suffix_ptr_const = T* const;

  // 7) T* volatile
  template<typename T> using suffix_ptr_volatile = T* volatile;

  // 8) T* const volatile
  template<typename T> using suffix_ptr_cv = T* const volatile;

  // 9) T[2]
  template<typename T> using suffix_array2 = T[2];

  // 10) T(&)()
  template<typename T> using suffix_fn = T();
  template<typename T> using suffix_fn_ref = T(&)();

  template<typename T>
  using all_suffixes = std::tuple<
    suffix_identity<T>,
    suffix_const<T>,
    suffix_volatile<T>,
    suffix_cv<T>,
    suffix_ptr<T>,
    suffix_ptr_const<T>,
    suffix_ptr_volatile<T>,
    suffix_ptr_cv<T>,
    suffix_array2<T>,
    suffix_fn<T>,
    suffix_fn_ref<T>
  >;
};