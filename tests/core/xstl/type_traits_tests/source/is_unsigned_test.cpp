#include <type_traits/is_unsigned.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_unsigned_test() {
  static_assert(xstl::is_unsigned<T>::value == Expected, "is_unsigned returned wrong value");
  static_assert(xstl::is_unsigned_v<T> == Expected, "is_unsigned_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_unsigned<T>::value == Expected, "is_unsigned returned wrong value");
}

NOYX_TEST(IsUnsigned, UnitTest) {
  tt_is_unsigned_test<unsigned char, true>();
  tt_is_unsigned_test<unsigned short, true>();
  tt_is_unsigned_test<unsigned int, true>();
  tt_is_unsigned_test<unsigned long, true>();
  tt_is_unsigned_test<unsigned long long, true>();

  tt_is_unsigned_test<bool, false>(); 
  tt_is_unsigned_test<char, false>();
  tt_is_unsigned_test<signed char, false>();
  tt_is_unsigned_test<short, false>();
  tt_is_unsigned_test<int, false>();
  tt_is_unsigned_test<long, false>();
  tt_is_unsigned_test<long long, false>();

  tt_is_unsigned_test<float, false>();
  tt_is_unsigned_test<double, false>();
  tt_is_unsigned_test<void, false>();
  tt_is_unsigned_test<int*, false>();
}
