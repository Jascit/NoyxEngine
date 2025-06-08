#pragma once
#include <remove_reference.hpp>
#include <remove_cv.hpp>

namespace xstl {  
  template<typename T>
  using remove_cvref_t = xstl::remove_cv_t<xstl::remove_reference_t<T>>;
  
  template <typename _Ty>
  struct remove_cvref {
    using type = remove_cvref_t<_Ty>;
  };
} // xstl