#pragma once
#include <type_traits/remove_reference.hpp>
#include <type_traits/remove_cv.hpp>

namespace xstl {  
  template <typename T>
  struct remove_cvref {
    using type = xstl::remove_cv_t<xstl::remove_reference_t<T>>;
  };

  template<typename T>
  using remove_cvref_t = xstl::remove_cvref<T>::type;
  
} // xstl