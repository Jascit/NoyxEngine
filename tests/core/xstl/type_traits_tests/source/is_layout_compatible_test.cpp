#include <type_traits/is_layout_compatible.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T, typename U>
constexpr void tt_is_layout_compatible_test_value(bool expected) {
  constexpr bool actual = xstl::is_layout_compatible<T, U>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_layout_compatible<T, U> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_layout_compatible<T, U> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
    << ", U = " << typeid(U).name()
  );
#endif
}


struct IsLayoutCompatibleUnitTest_A {
  int x;
  double y;
};

struct IsLayoutCompatibleUnitTest_B {
  int x;
  double y;
};

struct IsLayoutCompatibleUnitTest_C {
  double y;
  int x;
};

struct IsLayoutCompatibleUnitTest_D {
  int x;
  double y;
  char z;
};

struct TestTypeInvokerIsLayoutCompatible {
  constexpr void operator()() const {
    tt_is_layout_compatible_test_value<int, int>(true);
    tt_is_layout_compatible_test_value<IsLayoutCompatibleUnitTest_A, IsLayoutCompatibleUnitTest_A>(true);

    tt_is_layout_compatible_test_value<IsLayoutCompatibleUnitTest_A, IsLayoutCompatibleUnitTest_B>(true);
    tt_is_layout_compatible_test_value<IsLayoutCompatibleUnitTest_A, IsLayoutCompatibleUnitTest_C>(false);

    tt_is_layout_compatible_test_value<IsLayoutCompatibleUnitTest_A, IsLayoutCompatibleUnitTest_D>(false);

    tt_is_layout_compatible_test_value<int, double>(false);
  }
};

NOYX_TEST(IsLayoutCompatible, UnitTest) {
  TestTypeInvokerIsLayoutCompatible{}();
}
