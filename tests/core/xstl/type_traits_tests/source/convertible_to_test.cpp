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
    "convertible_to<From, To> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", From = " << typeid(From).name()
    << ", To = " << typeid(To).name()
  );
#endif
}

struct ConvertibleToUnitTestA {};
struct ConvertibleToUnitTestB {
  ConvertibleToUnitTestB(const ConvertibleToUnitTestA&) {} 
};

struct ConvertibleToUnitTestC {
  explicit ConvertibleToUnitTestC(const ConvertibleToUnitTestA&) {}
};

struct ConvertibleToUnitTestD {};

struct TestTypeInvokerConvertibleTo {
  constexpr void operator()() const {
    tt_convertible_to_test_value<int, double>(true);     
    tt_convertible_to_test_value<double, int>(true);        
    tt_convertible_to_test_value<int, char>(true);
    tt_convertible_to_test_value<int, void>(false);

    tt_convertible_to_test_value<int*, void*>(true);
    tt_convertible_to_test_value<void*, int*>(false);
    tt_convertible_to_test_value<int*, const void*>(true);

    tt_convertible_to_test_value<ConvertibleToUnitTestA, ConvertibleToUnitTestB>(true);    
    tt_convertible_to_test_value<ConvertibleToUnitTestA, ConvertibleToUnitTestC>(false);   
    tt_convertible_to_test_value<ConvertibleToUnitTestA, ConvertibleToUnitTestD>(false);   

    tt_convertible_to_test_value<int, int>(true);
    tt_convertible_to_test_value<ConvertibleToUnitTestB, ConvertibleToUnitTestB>(true);
  }
};

NOYX_TEST(ConvertibleTo, UnitTest) {
  TestTypeInvokerConvertibleTo{}(); 
}
