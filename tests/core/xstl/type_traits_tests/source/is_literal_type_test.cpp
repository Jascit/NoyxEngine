#include <type_traits/is_literal_type.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_literal_type_test_value(bool expected) {
  constexpr bool actual = xstl::is_literal_type<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_literal_type<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_literal_type<T> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
  );
#endif
}

struct TestTypeInvokerIsLiteralType {
  constexpr void operator()() const {
    struct LiteralType {
      int x;
      constexpr LiteralType(int v) : x(v) {}
    };

    struct NonLiteralType {
      int x;
      NonLiteralType() {}
    };

    struct NonLiteralTypeWithDtor {
      ~NonLiteralTypeWithDtor() {}
    };

    tt_is_literal_type_test_value<int>(true);
    tt_is_literal_type_test_value<double>(true);

    tt_is_literal_type_test_value<LiteralType>(true);
    tt_is_literal_type_test_value<NonLiteralType>(false);
    tt_is_literal_type_test_value<NonLiteralTypeWithDtor>(false);
  }
};

NOYX_TEST(IsLiteralType, UnitTest) {
  TestTypeInvokerIsLiteralType{}();
}
