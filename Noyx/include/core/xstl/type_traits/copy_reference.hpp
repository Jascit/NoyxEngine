#pragma once
#include "remove_reference.hpp"
#include "add_lvalue_reference.hpp"
#include "add_rvalue_reference.hpp"
#include "type_identity.hpp"

namespace xstl {
  template<typename From, typename To>
  struct copy_reference {
    using type = To;
  };

  template<typename From, typename To>
  struct copy_reference<From&, To> {
    using type = add_lvalue_reference_t<To>;
  };

  template<typename From, typename To>
  struct copy_reference<From&&, To> {
    using type = add_rvalue_reference_t<To>;
  };

  template<typename From, typename To>
  using copy_reference_t = typename copy_reference<From, To>::type;
} // xstl