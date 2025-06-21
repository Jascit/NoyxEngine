#include <tt_test_detail.hpp>
#include <type_traits/is_assignable.hpp>

template<typename To, typename From, bool Expected>
constexpr void tt_is_assignable_test_value() {
  constexpr bool actual = xstl::is_assignable_v<To, From>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_assignable<" #To ", " #From "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_assignable<" << typeid(To).name() << ", " << typeid(From).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsAssignable {
  constexpr void operator()() const {
    struct A {};
    struct B { B& operator=(const B&) = delete; };

    tt_is_assignable_test_value<int&, int, true>();
    tt_is_assignable_test_value<int&, const int, true>();
    tt_is_assignable_test_value<const int&, int, false>();
    tt_is_assignable_test_value<int, int, false>();
    tt_is_assignable_test_value<int*, int*, false>();
    tt_is_assignable_test_value<int*&, int*, true>();
    tt_is_assignable_test_value<const int*&, int*, true>();
    tt_is_assignable_test_value<A&, A, true>();
    tt_is_assignable_test_value<B&, B, false>();
  }
};

NOYX_TEST(IsAssignable, UnitTest) {
  TestTypeInvokerIsAssignable{}();
}