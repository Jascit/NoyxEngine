#pragma once
namespace xstl{
  template<typename T> 
  struct type_identity {
    using T type;
  };
  template<typename T>
  using type_identity_t = type_identity<T>::type;
}
