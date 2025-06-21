#include <tt_test_detail.hpp>
#include <type_traits/is_destructible.hpp>
#include <type_traits/is_complete.hpp>


template<typename T, bool Expected>
constexpr void tt_is_destructible_test_value() {
  constexpr bool actual = xstl::is_destructible_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_destructible<T> returned incorrect value"
  );
#else
  if constexpr (xstl::is_complete_v<T>) {
    NOYX_ASSERT_TRUE_MESSAGE(
      actual == Expected,
      "is_destructible<" << typeid(T).name() << "> returned incorrect: actual = " << actual
      << ", expected = " << Expected
    );
  }
  else {
    NOYX_ASSERT_TRUE_MESSAGE(
      actual == Expected,
      "is_destructible<" << "TestTypeInvokerIsDestructible::operator()::C" << "> returned incorrect: actual = " << actual
      << ", expected = " << Expected
    );
  }
#endif
}


struct TestTypeInvokerIsDestructible {
  constexpr void operator()() const {
    struct A { ~A() {} };
    struct B { B(const B&); ~B() = delete; };
    struct C;
    struct D { virtual ~D() = default; };

    tt_is_destructible_test_value<int, true>();
    tt_is_destructible_test_value<void, false>();
    tt_is_destructible_test_value<A, true>();
    tt_is_destructible_test_value<B, false>();
    tt_is_destructible_test_value<C, false>();
    tt_is_destructible_test_value<D, true>();
    tt_is_destructible_test_value<int&, true>();
    tt_is_destructible_test_value<int&&, true>();
    tt_is_destructible_test_value<int[3], true>();
    tt_is_destructible_test_value<void, false>();
    tt_is_destructible_test_value<int[3], true>();
    tt_is_destructible_test_value<int(*)[], true>();
    tt_is_destructible_test_value<int* (*)[5], true>();
  }
};

NOYX_TEST(IsDestructible, UnitTest) {
  TestTypeInvokerIsDestructible{}();
}
