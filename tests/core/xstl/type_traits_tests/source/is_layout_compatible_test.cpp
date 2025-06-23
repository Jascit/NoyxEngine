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


struct TestTypeInvokerIsLayoutCompatible {
  constexpr void operator()() const {
    struct A {
      int x;
      double y;
    };

    struct B {
      int x;
      double y;
    };

    struct C {
      double y;
      int x;
    };

    struct D {
      int x;
      double y;
      char z;
    };

    tt_is_layout_compatible_test_value<int, int>(true);
    tt_is_layout_compatible_test_value<A, A>(true);

    tt_is_layout_compatible_test_value<A, B>(true);
    tt_is_layout_compatible_test_value<A, C>(false);

    tt_is_layout_compatible_test_value<A, D>(false);

    tt_is_layout_compatible_test_value<int, double>(false);
  }
};

NOYX_TEST(IsLayoutCompatible, UnitTest) {
  TestTypeInvokerIsLayoutCompatible{}();
}
