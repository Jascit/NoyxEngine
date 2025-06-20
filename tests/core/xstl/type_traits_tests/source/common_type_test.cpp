#include <type_traits/common_type.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>  

template<typename T, typename U, typename Expected>
constexpr void tt_common_type_test_value() {
  constexpr bool actual = xstl::is_same<xstl::common_type_t<T, U>, Expected>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "common_type<> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "common_type<" << typeid(T).name() << ", " << typeid(U).name() << "> returned incorrect: "
    "actual = " << typeid(xstl::common_type_t<T, U>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerCommonType {
  constexpr void operator()() const {
    tt_common_type_test_value<int, int, int>();
    tt_common_type_test_value<const int, int, int>();
    tt_common_type_test_value<int&, int, int>();
    tt_common_type_test_value<int&&, int, int>();

    tt_common_type_test_value<int, double, double>();
    tt_common_type_test_value<const int&, double&, double>();

    tt_common_type_test_value<char*, const char*, const char*>();
    tt_common_type_test_value<const char*, char*, const char*>();

    tt_common_type_test_value<int[5], int*, int*>();

    tt_common_type_test_value<const volatile long&, volatile long&&, long>();
    tt_common_type_test_value<const volatile long&, volatile long, long>();


    struct A { operator double() const; };
    struct B { operator int() const; };
    tt_common_type_test_value<A, B, double>();

    using T1 = const int&;
    using T2 = volatile double&&;
    tt_common_type_test_value<T1, T2, double>();
  }
};

NOYX_TEST(CommonType, UnitTest) {
  TestTypeInvokerCommonType{}();
}