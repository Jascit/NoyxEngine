#include <tt_test_detail.hpp>
#include <type_traits/has_pre_increment.hpp>

template<typename T, bool Expected>
constexpr void tt_has_pre_increment_test_value() {
  constexpr bool actual = xstl::has_pre_increment<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_pre_increment<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_pre_increment<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasPreIncrement {
  void operator()() const {
    struct A { A& operator++(); };
    struct B {};
    struct C { void operator++(); }; 
    struct D { int operator++(); };
    struct E {
      E(const E&) = delete;
      E() = delete;
      E(E&&) = delete;
      int operator++();
    };
    struct F {
    private:
      int operator++();
    };

    tt_has_pre_increment_test_value<int, true>();
    tt_has_pre_increment_test_value<double, true>();
    tt_has_pre_increment_test_value<const int, false>(); 

    tt_has_pre_increment_test_value<A, true>();
    tt_has_pre_increment_test_value<B, false>();
    tt_has_pre_increment_test_value<C, true>(); 
    tt_has_pre_increment_test_value<D, true>();
    tt_has_pre_increment_test_value<E, true>();
    tt_has_pre_increment_test_value<F, false>();

    tt_has_pre_increment_test_value<int[3], false>();
    tt_has_pre_increment_test_value<void, false>();
  }
};

NOYX_TEST(HasPreIncrement, UnitTest) {
  TestTypeInvokerHasPreIncrement{}();
}
