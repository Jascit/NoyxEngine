#include <type_traits>

namespace xstl {
  template<typename T>
  struct class_of_member;

  template<typename D, typename C>
  struct class_of_member<D C::*>
  {
    using type = C;
  };

  template<typename T>
  using class_of_member_t = typename class_of_member<T>::type;
}