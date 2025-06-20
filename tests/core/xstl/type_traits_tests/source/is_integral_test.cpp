#include <type_traits/is_integral.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_integral_test() {
  static_assert(xstl::is_integral<T>::value == Expected, "is_integral returned wrong value");
  static_assert(xstl::is_integral_v<T> == Expected, "is_integral_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_integral<T>::value == Expected, "is_integral returned wrong value");
}

NOYX_TEST(IsIntegral, UnitTest) {
  tt_is_integral_test<bool, true>();
  tt_is_integral_test<char, true>();
  tt_is_integral_test<signed char, true>();
  tt_is_integral_test<unsigned char, true>();
  tt_is_integral_test<wchar_t, true>();
  tt_is_integral_test<char16_t, true>();
  tt_is_integral_test<char32_t, true>();
  tt_is_integral_test<short, true>();
  tt_is_integral_test<unsigned short, true>();
  tt_is_integral_test<int, true>();
  tt_is_integral_test<unsigned int, true>();
  tt_is_integral_test<long, true>();
  tt_is_integral_test<unsigned long, true>();
  tt_is_integral_test<long long, true>();
  tt_is_integral_test<unsigned long long, true>();

  tt_is_integral_test<float, false>();
  tt_is_integral_test<double, false>();
  tt_is_integral_test<long double, false>();
  tt_is_integral_test<void, false>();
  tt_is_integral_test<int*, false>();
  tt_is_integral_test<int&, false>();
  tt_is_integral_test<int&&, false>();
}
