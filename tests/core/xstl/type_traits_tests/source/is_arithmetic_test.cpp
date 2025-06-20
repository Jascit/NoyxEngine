#include <type_traits/is_arithmetic.hpp>
#include <type_traits/is_same.hpp>
#include <core/xstldef.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_arithmetic_test() {
  constexpr bool actual = xstl::is_arithmetic<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_arithmetic returned wrong value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_arithmetic<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

NOYX_TEST(IsArithmetic, UnitTest) {
  tt_is_arithmetic_test<int, true>();
  tt_is_arithmetic_test<unsigned int, true>();
  tt_is_arithmetic_test<long, true>();
  tt_is_arithmetic_test<unsigned long, true>();
  tt_is_arithmetic_test<signed char, true>();
  tt_is_arithmetic_test<unsigned char, true>();
  tt_is_arithmetic_test<char, true>();
  tt_is_arithmetic_test<bool, true>();

  tt_is_arithmetic_test<float, true>();
  tt_is_arithmetic_test<double, true>();
  tt_is_arithmetic_test<long double, true>();

  tt_is_arithmetic_test<void, false>();
  tt_is_arithmetic_test<int*, false>();
  tt_is_arithmetic_test<int&&, false>();
  tt_is_arithmetic_test<const int&&, false>();
  tt_is_arithmetic_test<const volatile int&&, false>();
  tt_is_arithmetic_test<const volatile int&, false>();
  tt_is_arithmetic_test<const volatile int[2], false>();
  tt_is_arithmetic_test<const volatile int[], false>();
  tt_is_arithmetic_test<const volatile int* const, false>();
  tt_is_arithmetic_test<const volatile int* const volatile, false>();
  tt_is_arithmetic_test<xstl::nullptr_t, false>();
}
