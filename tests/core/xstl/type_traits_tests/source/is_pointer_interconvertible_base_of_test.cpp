#include <tt_test_detail.hpp>
#include <type_traits/is_pointer_interconvertible_base_of.hpp>

template<typename Base, typename Derived>
constexpr void tt_is_pointer_interconvertible_base_of_test_value(bool expected) {
  constexpr bool actual = xstl::is_pointer_interconvertible_base_of_v<Base, Derived>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_pointer_interconvertible_base_of<" #Base ", " #Derived "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_pointer_interconvertible_base_of<" << typeid(Base).name()
    << ", " << typeid(Derived).name() << "> returned incorrect: actual = "
    << actual << ", expected = " << expected
  );
#endif
}


struct TestTypeInvokerIsPointerInterconvertibleBaseOf {
  void operator()() const {
    struct A {};
    struct B : A {};
    struct C : virtual A {};
    struct D {};
    struct E : B, D {};
    struct F {};

    tt_is_pointer_interconvertible_base_of_test_value<A, B>(true);
    tt_is_pointer_interconvertible_base_of_test_value<A, C>(false);
    tt_is_pointer_interconvertible_base_of_test_value<B, A>(false);
    tt_is_pointer_interconvertible_base_of_test_value<B, D>(false);
    tt_is_pointer_interconvertible_base_of_test_value<A, E>(true);
    tt_is_pointer_interconvertible_base_of_test_value<int, int>(false);
    tt_is_pointer_interconvertible_base_of_test_value<A, F>(false);
  }
};

NOYX_TEST(IsPointerInterconvertibleBaseOf, UnitTest) {
  TestTypeInvokerIsPointerInterconvertibleBaseOf{}();
}
