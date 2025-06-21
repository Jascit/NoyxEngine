#include <tt_test_detail.hpp>
#include <type_traits/is_aggregate.hpp>

template<typename T, bool Expected>
constexpr void tt_is_aggregate_test_value() {
  constexpr bool actual = xstl::is_aggregate_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_aggregate<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_aggregate<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsAggregate {
  void operator()() const {
    struct A { int x; double y; };
    struct B { B() {} int x; };
    struct C { private: int x; public: double y; };
    union U { int a; float b; };

    tt_is_aggregate_test_value<int, false>();
    tt_is_aggregate_test_value<double, false>();
    tt_is_aggregate_test_value<int[3], true>();
    tt_is_aggregate_test_value<int[], true>();
    tt_is_aggregate_test_value<int*, false>();

    tt_is_aggregate_test_value<A, true>();
    tt_is_aggregate_test_value<B, false>();
    tt_is_aggregate_test_value<C, false>();
    tt_is_aggregate_test_value<U, true>();
  }
};

NOYX_TEST(IsAggregate, UnitTest) {
  TestTypeInvokerIsAggregate{}();
}