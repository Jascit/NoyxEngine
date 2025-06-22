#pragma once
#include "remove_reference.hpp"
#include "remove_cv.hpp"

namespace xstl {  
  template <typename T>
  struct remove_cvref {
    using type = remove_cv_t<remove_reference_t<T>>;
  };

  template<typename T>
  using remove_cvref_t = typename remove_cvref<T>::type;
  
} // xstl
