#include <tt_test_detail.hpp>
#include <type_traits/has_unary_plus.hpp>

template<typename T, bool Expected>
constexpr void tt_has_unary_plus_test_value() {
  constexpr bool actual = xstl::has_unary_plus<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_unary_plus<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_unary_plus<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasUnaryPlus {
  void operator()() const {
    struct A { A operator+() const; };
    struct B {};
    struct C { void operator+() const; };
    struct D { int operator+(); };
    struct E { int operator+() const; };
    struct F {
      F(const F&) = delete;
      F() = delete;
      F(F&&) = delete;
      int operator+() const;
    };
    struct G {
    private:
      int operator+();
    };

    tt_has_unary_plus_test_value<int, true>();
    tt_has_unary_plus_test_value<double, true>();
    tt_has_unary_plus_test_value<const int, true>();

    tt_has_unary_plus_test_value<A, true>();
    tt_has_unary_plus_test_value<B, false>();
    tt_has_unary_plus_test_value<C, true>();
    tt_has_unary_plus_test_value<D, true>();
    tt_has_unary_plus_test_value<E, true>();
    tt_has_unary_plus_test_value<F, true>();
    tt_has_unary_plus_test_value<G, false>();

    tt_has_unary_plus_test_value<int[3], false>();
    tt_has_unary_plus_test_value<void, false>();
  }
};

NOYX_TEST(HasUnaryPlus, UnitTest) {
  TestTypeInvokerHasUnaryPlus{}();
}
