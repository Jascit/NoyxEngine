#include <type_traits/is_signed.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_signed_test() {
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_signed_v<T> == Expected, "is_signed_v returned wrong value");
#else
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_signed_v<T> == Expected, "is_signed_v returned wrong value, type: " << typeid(T).name());
#endif
}

NOYX_TEST(IsSigned, UnitTest) {
  tt_is_signed_test<signed char, true>();
  tt_is_signed_test<short, true>();
  tt_is_signed_test<int, true>();
  tt_is_signed_test<long, true>();
  tt_is_signed_test<long long, true>();
  tt_is_signed_test<char, true>();
  tt_is_signed_test<float, true>();
  tt_is_signed_test<double, true>();

  tt_is_signed_test<bool, false>();
  tt_is_signed_test<unsigned char, false>();
  tt_is_signed_test<unsigned short, false>();
  tt_is_signed_test<unsigned int, false>();
  tt_is_signed_test<unsigned long, false>();
  tt_is_signed_test<unsigned long long, false>();

  tt_is_signed_test<void, false>();
  tt_is_signed_test<int*, false>();
}
