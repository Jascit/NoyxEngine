#pragma once
#include "copy_reference.hpp"
#include "copy_cv.hpp"

namespace xstl {
  namespace details {
    template<typename From, typename To, typename FromRmRef = remove_reference_t<From>, typename ToRmRef = remove_reference_t<To>>
    struct _copy_cv_ref_impl {
      using type = copy_reference_t<From, copy_cv_t<FromRmRef, ToRmRef>>;
    };
  } // details
  template<typename From, typename To>
  struct copy_cv_ref : details::_copy_cv_ref_impl<From, To> {};

  template<typename From, typename To>
  using copy_cv_ref_t = typename copy_cv_ref<From, To>::type;
} // xstl