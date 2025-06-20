#include <type_traits/is_empty.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_empty_test_value(bool expected) {
  constexpr bool actual = xstl::is_empty<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_empty<T> returned incorrect value"
  );
}

struct IsEmptyUnitTest_Empty1 {};

struct IsEmptyUnitTest_Empty2 : IsEmptyUnitTest_Empty1 {};

struct IsEmptyUnitTest_NonEmpty1 {
  int x;
};

struct IsEmptyUnitTest_NonEmpty2 : IsEmptyUnitTest_Empty1 {
  int y;
};

struct IsEmptyUnitTest_WithVirtual {
  virtual void foo() {}
};

struct TestTypeInvokerIsEmpty {
  constexpr void operator()() const {
    tt_is_empty_test_value<int>(false);
    tt_is_empty_test_value<IsEmptyUnitTest_Empty1>(true);
    tt_is_empty_test_value<IsEmptyUnitTest_Empty2>(true);
    tt_is_empty_test_value<IsEmptyUnitTest_NonEmpty1>(false);
    tt_is_empty_test_value<IsEmptyUnitTest_NonEmpty2>(false);
    tt_is_empty_test_value<IsEmptyUnitTest_WithVirtual>(false);
  }
};

NOYX_TEST(IsEmpty, UnitTest) {
  TestTypeInvokerIsEmpty{}();
}
