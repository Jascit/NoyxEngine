#include <type_traits/is_reference.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_reference_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_reference<T>::value == Expected, "is_reference returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_reference_v<T> == Expected, "is_reference_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_reference<T>::value == Expected, "is_reference returned wrong value");
}

NOYX_TEST(IsReference, UnitTest) {
  tt_is_reference_test<int, false>();
  tt_is_reference_test<int&, true>();
  tt_is_reference_test<int&&, true>();
  tt_is_reference_test<const int&, true>();
  tt_is_reference_test<int*, false>();
  tt_is_reference_test<int*&, true>();
}
