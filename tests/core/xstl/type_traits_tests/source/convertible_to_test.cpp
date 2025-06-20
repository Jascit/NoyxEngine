#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>
#include <type_traits/convertible_to.hpp> 

template<typename From, typename To>
constexpr void tt_convertible_to_test_value(bool expected) {
  constexpr bool actual = xstl::convertible_to<From, To>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == expected)),
    "convertible_to<From, To> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == expected)),
    "convertible_to<From, To> returned incorrect value: got different from expected"
  );
#endif
}

struct A {};
struct B {
  B(const A&) {} 
};

struct C {
  explicit C(const A&) {}
};

struct D {};

struct TestTypeInvokerConvertibleTo {
  constexpr void operator()() const {
    tt_convertible_to_test_value<int, double>(true);     
    tt_convertible_to_test_value<double, int>(true);        
    tt_convertible_to_test_value<int, char>(true);
    tt_convertible_to_test_value<int, void>(false);

    tt_convertible_to_test_value<int*, void*>(true);
    tt_convertible_to_test_value<void*, int*>(false);
    tt_convertible_to_test_value<int*, const void*>(true);

    tt_convertible_to_test_value<A, B>(true);    
    tt_convertible_to_test_value<A, C>(false);   
    tt_convertible_to_test_value<A, D>(false);   

    tt_convertible_to_test_value<int, int>(true);
    tt_convertible_to_test_value<B, B>(true);
  }
};

NOYX_TEST(ConvertibleTo, UnitTest) {
  TestTypeInvokerConvertibleTo{}(); 
}
