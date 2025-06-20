#include <type_traits/is_fundamental.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp> 

template<typename T, bool Expected>
constexpr void tt_is_fundamental_test_type() {
  constexpr bool actual = xstl::is_fundamental_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_fundamental<T> returned incorrect result"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_fundamental<T> returned incorrect result: actual = " << actual
    << ", expected = " << Expected
  );
#endif
}

template<typename T>
struct TestTypeInvokerIsFundamental {
  constexpr void operator()() const {
    tt_is_fundamental_test_type<T, true>();
    tt_is_fundamental_test_type<const T, true>();
    tt_is_fundamental_test_type<volatile T, true>();
    tt_is_fundamental_test_type<const volatile T, true>();
  }
};

template<typename T>
struct TestTypeInvokerIsNotFundamental {
  constexpr void operator()() const {
    tt_is_fundamental_test_type<T, false>();
  }
};

NOYX_TEST(IsFundamental, UnitTest) {
  TestTypeInvokerIsFundamental<int>()();
  TestTypeInvokerIsFundamental<double>()();
  TestTypeInvokerIsFundamental<char>()();
  TestTypeInvokerIsFundamental<void>()();
  TestTypeInvokerIsFundamental<std::nullptr_t>()();

  TestTypeInvokerIsNotFundamental<int*>()();
  TestTypeInvokerIsNotFundamental<void(*)()>()();
  TestTypeInvokerIsNotFundamental<int[3]>()();
  TestTypeInvokerIsNotFundamental<struct Dummy>()();
}
