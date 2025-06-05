#pragma once
namespace xstl {
  template<typename T>
  struct remove_reference {
    using type = T;
  };
  //lvalue
  template<typename T>
  struct remove_reference<T&> {
    using type = T;
  };
  //rvalue
  template<typename T>
  struct remove_reference<T&&> {
    using type = T;
  };

  template<typename T>
  using remove_reference_t = typename remove_reference<T>::type;

} // namespaces xstl

