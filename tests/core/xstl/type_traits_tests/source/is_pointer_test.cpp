#include <type_traits/is_pointer.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_pointer_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_pointer<T>::value == Expected, "is_pointer returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_pointer_v<T> == Expected, "is_pointer_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_pointer<T>::value == Expected, "is_pointer returned wrong value");
}

NOYX_TEST(IsPointer, UnitTest) {
  tt_is_pointer_test<int, false>();
  tt_is_pointer_test<int*, true>();
  tt_is_pointer_test<const int*, true>();
  tt_is_pointer_test<int* const, true>();
  tt_is_pointer_test<int* volatile, true>();
  tt_is_pointer_test<int* const volatile, true>();

  tt_is_pointer_test<void*, true>();
  tt_is_pointer_test<int&, false>();
  tt_is_pointer_test<int&&, false>();
  tt_is_pointer_test<int**, true>();
}
