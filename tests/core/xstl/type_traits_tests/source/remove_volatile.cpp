#include <tt_test_detail.hpp>
#include <type_traits/remove_volatile.hpp>
#include <type_traits/is_same.hpp>
#include <iostream>

template<typename T>
constexpr void tt_remove_volatile_test_type() {
  using Removed = xstl::remove_volatile_t<T>;
  using Expected = typename std::remove_volatile<T>::type; 
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<Removed, Expected>)), "remove_volatile failed");
}

template<typename T>
struct TestTypeInvokerRemoveVolatile {
  constexpr void operator()() const {
    tt_remove_volatile_test_type<T>();
  }
};



NOYX_TEST(RemoveVolatile, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerRemoveVolatile>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerRemoveVolatile>(xstl_test_detail::all_test_types{});
}
