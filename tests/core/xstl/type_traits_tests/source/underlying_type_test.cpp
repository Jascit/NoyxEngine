#include <tt_test_detail.hpp>
#include <type_traits/underlying_type.hpp>

template<typename T, typename Expected>
constexpr void tt_underlying_type_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::underlying_type_t<T>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "underlying_type<" #T "> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "underlying_type<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << typeid(xstl::underlying_type_t<T>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}


struct TestTypeInvokerUnderlyingType {
  constexpr void operator()() const {
    enum E1 : unsigned short { A1, B1 };
    enum class E2 : long { A2, B2 };
    enum E3 { A3, B3 };

    tt_underlying_type_test_value<E1, unsigned short>();
    tt_underlying_type_test_value<E2, long>();
    tt_underlying_type_test_value<E3, int>();
  }
};

NOYX_TEST(UnderlyingType, UnitTest) {
  TestTypeInvokerUnderlyingType{}();
}