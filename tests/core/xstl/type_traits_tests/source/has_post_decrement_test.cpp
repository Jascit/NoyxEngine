#include <tt_test_detail.hpp>
#include <type_traits/has_post_decrement.hpp>

template<typename T, bool Expected>
constexpr void tt_has_post_decrement_test_value() {
  constexpr bool actual = xstl::has_post_decrement<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_post_decrement<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_post_decrement<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasPostDecrement {
  void operator()() const {
    struct A { A operator--(int); };
    struct B {};
    struct C { void operator--(int); };
    struct D { int operator--(int); };
    struct E {
      E(const E&) = delete;
      E() = delete;
      E(E&&) = delete;
      int operator--(int);
    };
    struct F {
    private:
      int operator--(int);
    };

    tt_has_post_decrement_test_value<int, true>();
    tt_has_post_decrement_test_value<double, true>();
    tt_has_post_decrement_test_value<const int, false>(); 

    tt_has_post_decrement_test_value<A, true>();
    tt_has_post_decrement_test_value<B, false>();
    tt_has_post_decrement_test_value<C, true>();
    tt_has_post_decrement_test_value<D, true>();
    tt_has_post_decrement_test_value<E, true>();
    tt_has_post_decrement_test_value<F, false>();

    tt_has_post_decrement_test_value<int[3], false>();
    tt_has_post_decrement_test_value<void, false>();
  }
};

NOYX_TEST(HasPostDecrement, UnitTest) {
  TestTypeInvokerHasPostDecrement{}();
}
