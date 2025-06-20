#include <type_traits/is_polymorphic.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_is_polymorphic_test_value(bool expected) {
  constexpr bool actual = xstl::is_polymorphic<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "is_polymorphic<T> returned incorrect value"
  );
}


struct IsPolymorphicUnitTest_NonPolymorphic {
  void foo() {}
};

struct IsPolymorphicUnitTest_Polymorphic {
  virtual void foo() {}
};

struct IsPolymorphicUnitTest_DerivedPolymorphic : IsPolymorphicUnitTest_Polymorphic {};

struct IsPolymorphicUnitTest_PolymorphicWithVirtualDtor {
  virtual ~IsPolymorphicUnitTest_PolymorphicWithVirtualDtor() {}
};

struct TestTypeInvokerIsPolymorphic {
  constexpr void operator()() const {
    tt_is_polymorphic_test_value<int>(false);
    tt_is_polymorphic_test_value<IsPolymorphicUnitTest_NonPolymorphic>(false);
    tt_is_polymorphic_test_value<IsPolymorphicUnitTest_Polymorphic>(true);
    tt_is_polymorphic_test_value<IsPolymorphicUnitTest_DerivedPolymorphic>(true);
    tt_is_polymorphic_test_value<IsPolymorphicUnitTest_PolymorphicWithVirtualDtor>(true);
  }
};

NOYX_TEST(IsPolymorphic, UnitTest) {
  TestTypeInvokerIsPolymorphic{}();
}
