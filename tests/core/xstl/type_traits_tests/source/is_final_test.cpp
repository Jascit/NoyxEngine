#include <type_traits/is_final.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_final_test_value(bool expected) {
  constexpr bool actual = xstl::is_final<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_final<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_final<T> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
  );
#endif
}

struct TestTypeInvokerIsFinal {
  constexpr void operator()() const {
    struct NotFinal {};
    struct Final final {};
    struct DerivedFromNotFinal : NotFinal {};

    tt_is_final_test_value<int>(false);
    tt_is_final_test_value<NotFinal>(false);
    tt_is_final_test_value<Final>(true);
    tt_is_final_test_value<DerivedFromNotFinal>(false);
  }
};

NOYX_TEST(IsFinal, UnitTest) {
  TestTypeInvokerIsFinal{}();
}
