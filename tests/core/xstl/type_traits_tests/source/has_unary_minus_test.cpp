#include <tt_test_detail.hpp>
#include <type_traits/has_unary_minus.hpp>

template<typename T, bool Expected>
constexpr void tt_has_unary_minus_test_value() {
  constexpr bool actual = xstl::has_unary_minus<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_unary_minus<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_unary_minus<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasUnaryMinus {
  void operator()() const {
    struct A { A operator-() const; };
    struct B {};
    struct C { void operator-() const; }; // invalid return type
    struct D { int operator-(); };
    struct E { int operator-() const; };
    struct F {
    private:
      int operator-();
    };

    tt_has_unary_minus_test_value<int, true>();
    tt_has_unary_minus_test_value<double, true>();
    tt_has_unary_minus_test_value<const int, true>();

    tt_has_unary_minus_test_value<A, true>();
    tt_has_unary_minus_test_value<B, false>();
    tt_has_unary_minus_test_value<C, true>(); // все ще вал≥дний вираз Ч return type не важливий
    tt_has_unary_minus_test_value<D, true>();
    tt_has_unary_minus_test_value<E, true>();
    tt_has_unary_minus_test_value<F, false>(); // оператор приватний

    tt_has_unary_minus_test_value<int[3], false>();
    tt_has_unary_minus_test_value<void, false>();
  }
};

NOYX_TEST(HasUnaryMinus, UnitTest) {
  TestTypeInvokerHasUnaryMinus{}();
}