#include <type_traits/has_virtual_destructor.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_has_virtual_destructor_test_value(bool expected) {
  constexpr bool actual = xstl::has_virtual_destructor<T>::value;

  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "has_virtual_destructor<T> returned incorrect value"
  );
}

struct HasVirtualDestructorUnitTest_NoVirtualDtor {
  ~NoVirtualDtor() {}
};

struct HasVirtualDestructorUnitTest_VirtualDtor {
  virtual ~VirtualDtor() {}
};

struct HasVirtualDestructorUnitTestDerived_VirtualDtor : HasVirtualDestructorUnitTest_VirtualDtor {};

struct HasVirtualDestructorUnitTest_NoDestructor {}; 

struct TestTypeInvokerHasVirtualDestructor {
  constexpr void operator()() const {
    tt_has_virtual_destructor_test_value<int>(false); 
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_NoVirtualDtor>(false);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_VirtualDtor>(true);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_DerivedVirtualDtor>(true);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_NoDestructor>(false);
  }
};

NOYX_TEST(HasVirtualDestructor, UnitTest) {
  TestTypeInvokerHasVirtualDestructor{}();
}
