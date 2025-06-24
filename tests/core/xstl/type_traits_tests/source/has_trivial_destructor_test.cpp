#include <tt_test_detail.hpp>
#include <type_traits/has_trivial_destructor.hpp> 

template<typename T, bool Expected>
constexpr void tt_has_trivial_destructor_test_value() {
  constexpr bool actual = xstl::has_trivial_destructor<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_trivial_destructor<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_trivial_destructor<" << typeid(T).name()
    << "> returned incorrect: actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasTrivialDestructor {
  void operator()() const {
    struct A {};                       
    struct B { int x; };               
    struct C { ~C() {} };               
    struct D { ~D() = default; };      
    struct E {
      ~E();
    };                                  
    struct F {
      F(const F&) = delete;
      F() = delete;
      ~F() = default;
    };                                
    struct G {
      virtual void foo();
    };                                  
    struct H {
      virtual ~H() = default;
    };                                  
    struct I {
      int x;
    private:
      ~I() = default;                 
    };

    tt_has_trivial_destructor_test_value<int, true>();
    tt_has_trivial_destructor_test_value<double, true>();
    tt_has_trivial_destructor_test_value<void, false>();

    tt_has_trivial_destructor_test_value<A, true>();
    tt_has_trivial_destructor_test_value<B, true>();
    tt_has_trivial_destructor_test_value<C, false>();
    tt_has_trivial_destructor_test_value<D, true>();
    tt_has_trivial_destructor_test_value<E, false>();
    tt_has_trivial_destructor_test_value<F, true>();
    tt_has_trivial_destructor_test_value<G, false>();
    tt_has_trivial_destructor_test_value<H, false>();
    tt_has_trivial_destructor_test_value<I, true>();
  }
};

NOYX_TEST(HasTrivialDestructor, UnitTest) {
  TestTypeInvokerHasTrivialDestructor{}();
}
