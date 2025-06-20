#include <type_traits/is_base_of.hpp> 
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename Base, typename Derived>
constexpr void tt_is_base_of_test_value(bool expected) {
  constexpr bool actual = xstl::is_base_of<Base, Derived>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_base_of<Base, Derived> returned incorrect value"
  );
}

struct Base {};
struct DerivedPublic : Base {};
struct DerivedPrivate : private Base {};
struct Unrelated {};

struct TestTypeInvokerIsBaseOf {
  constexpr void operator()() const {
    tt_is_base_of_test_value<Base, DerivedPublic>(true);
    tt_is_base_of_test_value<Base, DerivedPrivate>(true);
    tt_is_base_of_test_value<Base, Base>(true);  

    tt_is_base_of_test_value<Base, Unrelated>(false);
    tt_is_base_of_test_value<DerivedPublic, Base>(false);
    tt_is_base_of_test_value<int, int>(false);
  }
};

NOYX_TEST(IsBaseOf, UnitTest) {
  TestTypeInvokerIsBaseOf{}();
}
