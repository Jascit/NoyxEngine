#include <type_traits/type_identity.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits>
#include <iostream>
#include <tt_test_detail.hpp>

template<typename T>
constexpr void tt_type_identity_test_type() {
  using TI = xstl::type_identity<T>;
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<typename TI::type, T>)), "type_identity failed");
}

template<typename T>
struct TestInvokerTypeIdentity {
  constexpr void operator()() const {
    tt_type_identity_test_type<T>();
  }
};


NOYX_TEST(TypeIdentity, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestInvokerTypeIdentity>();
  xstl_test_detail::test_all_types_with_suffix<0, TestInvokerTypeIdentity>(xstl_test_detail::all_test_types{});
}

