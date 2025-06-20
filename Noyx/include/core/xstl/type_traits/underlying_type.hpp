#pragma once

namespace xstl {
  namespace details {
    template<typename T, bool = is_enum_v<T>>
    struct _underlying_type_helper {
      using type = __underlying_type(T);
    };

    template<typename T>
    struct _underlying_type_helper<T, false> {};
  }
  
  template<typename T>
  struct underlying_type : details::_underlying_type_helper<T> {};

  template<typename T>
  using underlying_type_t = underlying_type<T>::type;
}
