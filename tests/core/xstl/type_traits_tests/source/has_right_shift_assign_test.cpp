#include <tt_test_detail.hpp>
#include <type_traits/has_right_shift_assign.hpp>

template<typename T, typename U, bool Expected>
constexpr void tt_has_right_shift_assign_test_value() {
  constexpr bool actual = xstl::has_right_shift_assign<T, U>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_right_shift_assign<" #T ", " #U "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_right_shift_assign<" << typeid(T).name()
    << ", " << typeid(U).name()
    << "> returned incorrect: actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasRightShiftAssign {
  void operator()() const {
    struct A { A& operator>>=(int); };
    struct B {};
    struct C { void operator>>=(int); };
    struct D { int operator>>=(double); };
    struct E {
      E(const E&) = delete;
      E() = delete;
      E(E&&) = delete;
      int operator>>=(int);
    };
    struct F {
    private:
      int operator>>=(int);
    };

    tt_has_right_shift_assign_test_value<int, int, true>();
    tt_has_right_shift_assign_test_value<unsigned, int, true>();
    tt_has_right_shift_assign_test_value<int, unsigned, true>();
    tt_has_right_shift_assign_test_value<const int, int, false>();

    tt_has_right_shift_assign_test_value<A, int, true>();
    tt_has_right_shift_assign_test_value<B, int, false>();
    tt_has_right_shift_assign_test_value<C, int, true>();  
    tt_has_right_shift_assign_test_value<D, double, true>(); 
    tt_has_right_shift_assign_test_value<D, int, true>();  
    tt_has_right_shift_assign_test_value<E, int, true>();    
    tt_has_right_shift_assign_test_value<F, int, false>();  

    tt_has_right_shift_assign_test_value<int[3], int, false>(); 
    tt_has_right_shift_assign_test_value<void, int, false>();  
  }
};

NOYX_TEST(HasRightShiftAssign, UnitTest) {
  TestTypeInvokerHasRightShiftAssign{}();
}
