#include <type_traits/is_compound.hpp>
#include <tt_test_detail.hpp>

template<typename T, bool Expected>
constexpr void tt_is_compound_test_value() {
  constexpr bool actual1 = xstl::is_compound<T>::value;
  constexpr bool actual2 = xstl::is_compound_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_compound<" #T "> returned incorrect value"
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_compound_v<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_compound<" << typeid(T).name()
    << "> actual = " << actual1 << ", expected = " << Expected
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_compound_v<" << typeid(T).name()
    << "> actual = " << actual2 << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsCompound {
  constexpr void operator()() const {
    tt_is_compound_test_value<int, false>();
    tt_is_compound_test_value<const float, false>();
    tt_is_compound_test_value<bool, false>();
    tt_is_compound_test_value<void, false>();

    tt_is_compound_test_value<int*, true>();
    tt_is_compound_test_value<double&, true>();
    tt_is_compound_test_value<char[4], true>();
    tt_is_compound_test_value<void(), true>();

    struct S { int x; };
    tt_is_compound_test_value<S, true>();
    tt_is_compound_test_value<S*, true>();

    enum E { A, B };
    tt_is_compound_test_value<E, true>();

    tt_is_compound_test_value<int S::*, true>();

    tt_is_compound_test_value<const int*, true>();
    tt_is_compound_test_value<volatile S&, true>();

    tt_is_compound_test_value<void(*)(int), true>();
  }
};

NOYX_TEST(IsCompound, UnitTest) {
  TestTypeInvokerIsCompound{}();
}
