#include <type_traits/is_convertible.hpp> 
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename From, typename To>
constexpr void tt_is_convertible_test_value(bool expected) {
  constexpr bool actual = xstl::is_convertible<From, To>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_convertible<From, To> returned incorrect value"
  );
}

struct A {};
struct B {
  B(const A&) {}  
};

struct C {
  explicit C(const A&) {} 
};

struct TestTypeInvokerIsConvertible {
  constexpr void operator()() const {
    tt_is_convertible_test_value<int, double>(true);
    tt_is_convertible_test_value<double, int>(true);
    tt_is_convertible_test_value<int, int>(true);
    tt_is_convertible_test_value<int, void*>(false);

    tt_is_convertible_test_value<A, B>(true);
    tt_is_convertible_test_value<A, C>(false); 

    tt_is_convertible_test_value<int*, void*>(true);
    tt_is_convertible_test_value<void*, int*>(false);
  }
};

NOYX_TEST(IsConvertible, UnitTest) {
  TestTypeInvokerIsConvertible{}();
}
