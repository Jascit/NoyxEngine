#include <type_traits/is_literal_type.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_literal_type_test_value(bool expected) {
  constexpr bool actual = xstl::is_literal_type<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_literal_type<T> returned incorrect value"
  );
}

struct IsLiteralTypeUnitTest_LiteralType {
  int x;
  constexpr IsLiteralTypeUnitTest_LiteralType(int v) : x(v) {}
};

struct IsLiteralTypeUnitTest_NonLiteralType {
  int x;
  IsLiteralTypeUnitTest_NonLiteralType() {}
};

struct IsLiteralTypeUnitTest_NonLiteralTypeWithDtor {
  ~IsLiteralTypeUnitTest_NonLiteralTypeWithDtor() {}
};

struct TestTypeInvokerIsLiteralType {
  constexpr void operator()() const {
    tt_is_literal_type_test_value<int>(true);
    tt_is_literal_type_test_value<double>(true);

    tt_is_literal_type_test_value<IsLiteralTypeUnitTest_LiteralType>(true);

    tt_is_literal_type_test_value<IsLiteralTypeUnitTest_NonLiteralType>(false);

    tt_is_literal_type_test_value<IsLiteralTypeUnitTest_NonLiteralTypeWithDtor>(false);
  }
};

NOYX_TEST(IsLiteralType, UnitTest) {
  TestTypeInvokerIsLiteralType{}();
}
