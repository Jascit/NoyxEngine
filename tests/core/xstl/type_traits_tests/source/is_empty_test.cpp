#include <type_traits/is_empty.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_empty_test_value(bool expected) {
  constexpr bool actual = xstl::is_empty<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_empty<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_empty<T> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
  );
#endif
}

struct TestTypeInvokerIsEmpty {
  constexpr void operator()() const {
    struct Empty1 {};
    struct Empty2 : Empty1 {};
    struct NonEmpty1 { int x; };
    struct NonEmpty2 : Empty1 {int y;};
    struct WithVirtual { virtual void foo() {}};

    tt_is_empty_test_value<int>(false);
    tt_is_empty_test_value<Empty1>(true);
    tt_is_empty_test_value<Empty2>(true);
    tt_is_empty_test_value<NonEmpty1>(false);
    tt_is_empty_test_value<NonEmpty2>(false);
    tt_is_empty_test_value<WithVirtual>(false);
  }
};

NOYX_TEST(IsEmpty, UnitTest) {
  TestTypeInvokerIsEmpty{}();
}
