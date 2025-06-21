#include <tt_test_detail.hpp>
#include <type_traits/is_standard_layout.hpp>

template<typename T, bool Expected>
constexpr void tt_is_standard_layout_test_value() {
  constexpr bool actual = xstl::is_standard_layout_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_standard_layout<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "is_standard_layout<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsStandardLayout {
  void operator()() const {
    struct A { int x; double y; };
    struct B { virtual void f() {}; };
    union U { int a; float b; };
    enum E { E0, E1 };
    struct Base { int b; };
    struct Derived : Base { int d; };
    struct Mixed { private: int i; public: double d; };

    tt_is_standard_layout_test_value<int, true>();
    tt_is_standard_layout_test_value<double, true>();
    tt_is_standard_layout_test_value<int&, false>();
    tt_is_standard_layout_test_value<int*, true>();
    tt_is_standard_layout_test_value<int[5], true>();

    tt_is_standard_layout_test_value<A, true>();
    tt_is_standard_layout_test_value<B, false>();
    tt_is_standard_layout_test_value<U, true>();
    tt_is_standard_layout_test_value<E, true>();

    tt_is_standard_layout_test_value<Base, true>();
    tt_is_standard_layout_test_value<Derived, false>();

    tt_is_standard_layout_test_value<Mixed, false>();
  }
};

NOYX_TEST(IsStandardLayout, UnitTest) {
  TestTypeInvokerIsStandardLayout{}();
}
