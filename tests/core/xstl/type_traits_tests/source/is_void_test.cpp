#include <type_traits/is_void.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_void_test() {
  static_assert(xstl::is_void<T>::value == Expected, "is_void returned wrong value");
  static_assert(xstl::is_void_v<T> == Expected, "is_void_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_void<T>::value == Expected, "is_void returned wrong value");
}

NOYX_TEST(IsVoid, UnitTest) {
  tt_is_void_test<void, true>();
  tt_is_void_test<const void, true>();
  tt_is_void_test<volatile void, true>();
  tt_is_void_test<const volatile void, true>();

  tt_is_void_test<int, false>();
  tt_is_void_test<int*, false>();
  tt_is_void_test<float, false>();
  tt_is_void_test<void*, false>();
}
