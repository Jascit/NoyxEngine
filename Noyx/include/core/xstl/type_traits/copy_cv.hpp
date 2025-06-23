#pragma once

namespace xstl {
  template<typename From, typename To>
  struct copy_cv{
    using type = To;
  };
  
  template<typename From, typename To>
  struct copy_cv<const From, To>{
    using type = const To;
  };

  template<typename From, typename To>
  struct copy_cv<volatile From, To>{
    using type = volatile To;
  };

  template<typename From, typename To>
  struct copy_cv<const volatile From, To>{
    using type = const volatile To;
  };

  template<typename From, typename To>
  using copy_cv_t = typename copy_cv<From, To>::type;
  
}