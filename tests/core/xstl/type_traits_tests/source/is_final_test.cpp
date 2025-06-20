#include <type_traits/is_final.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_final_test_value(bool expected) {
  constexpr bool actual = xstl::is_final<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_final<T> returned incorrect value"
  );
}

struct IsFinalUnitTest_NotFinal {};

struct IsFinalUnitTest_Final final {};

struct IsFinalUnitTest_DerivedFromNotFinal : IsFinalUnitTest_NotFinal {};

struct TestTypeInvokerIsFinal {
  constexpr void operator()() const {
    tt_is_final_test_value<int>(false);
    tt_is_final_test_value<IsFinalUnitTest_NotFinal>(false);
    tt_is_final_test_value<IsFinalUnitTest_Final>(true);
    tt_is_final_test_value<IsFinalUnitTest_DerivedFromNotFinal>(false);
  }
};

NOYX_TEST(IsFinal, UnitTest) {
  TestTypeInvokerIsFinal{}();
}
