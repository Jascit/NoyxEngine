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

struct IsBaseOfUnitTest_Base {};
struct IsBaseOfUnitTest_DerivedPublic : IsBaseOfUnitTest_Base {};
struct IsBaseOfUnitTest_DerivedPrivate : private IsBaseOfUnitTest_Base {};
struct IsBaseOfUnitTest_Unrelated {};

struct TestTypeInvokerIsBaseOf {
  constexpr void operator()() const {
    tt_is_base_of_test_value<IsBaseOfUnitTest_Base, IsBaseOfUnitTest_DerivedPublic>(true);
    tt_is_base_of_test_value<IsBaseOfUnitTest_Base, IsBaseOfUnitTest_DerivedPrivate>(true);
    tt_is_base_of_test_value<IsBaseOfUnitTest_Base, IsBaseOfUnitTest_Base>(true);  

    tt_is_base_of_test_value<IsBaseOfUnitTest_Base, IsBaseOfUnitTest_Unrelated>(false);
    tt_is_base_of_test_value<IsBaseOfUnitTest_DerivedPublic, IsBaseOfUnitTest_Base>(false);
    tt_is_base_of_test_value<int, int>(false);
  }
};

NOYX_TEST(IsBaseOf, UnitTest) {
  TestTypeInvokerIsBaseOf{}();
}
