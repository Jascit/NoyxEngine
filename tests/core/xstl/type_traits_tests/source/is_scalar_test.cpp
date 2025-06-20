#include <type_traits/is_scalar.hpp> 
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T, bool Expected>
constexpr void tt_is_scalar_test_value() {
  constexpr bool actual = xstl::is_scalar_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_scalar<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_scalar<T> returned incorrect value: actual = " << actual
    << ", expected = " << Expected
    << ", T = " << typeid(T).name()
  );
#endif
}

template<typename T>
struct TestTypeInvokerIsScalar {
  constexpr void operator()() const {
    tt_is_scalar_test_value<int, true>();
    tt_is_scalar_test_value<const double, true>();
    tt_is_scalar_test_value<int*, true>();
    tt_is_scalar_test_value<std::nullptr_t, true>();
    enum Enum { A, B };
    tt_is_scalar_test_value<Enum, true>();

    struct MyStruct {};
    tt_is_scalar_test_value<MyStruct, false>();
    tt_is_scalar_test_value<int[3], false>();
    tt_is_scalar_test_value<void(int), false>();
    tt_is_scalar_test_value<int&, false>();
    tt_is_scalar_test_value<void, false>();
  }
};

NOYX_TEST(IsScalar, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerIsScalar>();
}
