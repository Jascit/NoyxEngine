#include <type_traits/is_array.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_array_test() {
  static_assert(xstl::is_array<T>::value == Expected, "is_array returned wrong value");
  static_assert(xstl::is_array_v<T> == Expected, "is_array_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_array<T>::value == Expected, "is_array returned wrong value");
}

NOYX_TEST(IsArray, UnitTest) {
  tt_is_array_test<int, false>();
  tt_is_array_test<int[], true>();
  tt_is_array_test<int[3], true>();
  tt_is_array_test<const int[5], true>();
  tt_is_array_test<int*, false>();
  tt_is_array_test<int&, false>();
  tt_is_array_test<int&&, false>();
}
