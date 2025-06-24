#include <tt_test_detail.hpp>
#include <type_traits/has_trivial_move_assign.hpp> 

template<typename T, bool Expected>
constexpr void tt_has_trivial_move_assign_test_value() {
  constexpr bool actual = xstl::has_trivial_move_assign<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_trivial_move_assign<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_trivial_move_assign<" << typeid(T).name()
    << "> returned incorrect: actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasTrivialMoveAssign {
  void operator()() const {
    struct A {}; 
    struct B { int x; }; 
    struct C {
      C& operator=(C&&);  
    };
    struct D {
      D& operator=(D&&) = default;  
    };
    struct E {
      E& operator=(D&&); 
    };
    struct F {
      F& operator=(F&&) = delete; 
    };
    struct G {
      virtual void foo() {};
    };
    struct H {
      H& operator=(const H&) = delete;
      H& operator=(H&&) = default;
    };

    tt_has_trivial_move_assign_test_value<int, true>();
    tt_has_trivial_move_assign_test_value<double, true>();
    tt_has_trivial_move_assign_test_value<void, false>();

    tt_has_trivial_move_assign_test_value<A, true>();
    tt_has_trivial_move_assign_test_value<B, true>();
    tt_has_trivial_move_assign_test_value<C, false>();
    tt_has_trivial_move_assign_test_value<D, true>();
    tt_has_trivial_move_assign_test_value<E, true>(); 
    tt_has_trivial_move_assign_test_value<F, false>();
    tt_has_trivial_move_assign_test_value<G, false>();
    tt_has_trivial_move_assign_test_value<H, true>();
  }
};

NOYX_TEST(HasTrivialMoveAssign, UnitTest) {
  TestTypeInvokerHasTrivialMoveAssign{}();
}
