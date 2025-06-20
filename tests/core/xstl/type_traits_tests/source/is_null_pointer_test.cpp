#include <type_traits/is_null_pointer.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_null_pointer_test() {
  static_assert(xstl::is_null_pointer<T>::value == Expected, "is_null_pointer returned wrong value");
  static_assert(xstl::is_null_pointer_v<T> == Expected, "is_null_pointer_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_null_pointer<T>::value == Expected, "is_null_pointer returned wrong value");
}

NOYX_TEST(IsNullPointer, UnitTest) {
  tt_is_null_pointer_test<std::nullptr_t, true>();
  tt_is_null_pointer_test<decltype(nullptr), true>();

  tt_is_null_pointer_test<int, false>();
  tt_is_null_pointer_test<void*, false>();
  tt_is_null_pointer_test<int*, false>();
  tt_is_null_pointer_test<std::nullptr_t*, false>();
}
