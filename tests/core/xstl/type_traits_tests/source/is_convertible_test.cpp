#include <type_traits/is_convertible.hpp> 
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename From, typename To>
constexpr void tt_is_convertible_test_value(bool expected) {
  constexpr bool actual = xstl::is_convertible<From, To>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_convertible<From, To> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_convertible<From, To> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", From = " << typeid(From).name()
    << ", To = " << typeid(To).name()
  );
#endif
}

struct IsConvertibleUnitTest_A {};
struct IsConvertibleUnitTest_B {
  IsConvertibleUnitTest_B(const IsConvertibleUnitTest_A&) {}  
};

struct IsConvertibleUnitTest_C {
  explicit IsConvertibleUnitTest_C(const IsConvertibleUnitTest_A&) {} 
};

struct TestTypeInvokerIsConvertible {
  constexpr void operator()() const {
    tt_is_convertible_test_value<int, double>(true);
    tt_is_convertible_test_value<double, int>(true);
    tt_is_convertible_test_value<int, int>(true);
    tt_is_convertible_test_value<int, void*>(false);

    tt_is_convertible_test_value<IsConvertibleUnitTest_A, IsConvertibleUnitTest_B>(true);
    tt_is_convertible_test_value<IsConvertibleUnitTest_A, IsConvertibleUnitTest_C>(false); 

    tt_is_convertible_test_value<int*, void*>(true);
    tt_is_convertible_test_value<void*, int*>(false);
  }
};

NOYX_TEST(IsConvertible, UnitTest) {
  TestTypeInvokerIsConvertible{}();
}
