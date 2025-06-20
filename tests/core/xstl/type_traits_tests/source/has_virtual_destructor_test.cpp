#include <type_traits/has_virtual_destructor.hpp>
#include <tt_test_detail.hpp>
#include <type_traits.hpp>

template<typename T>
constexpr void tt_has_virtual_destructor_test_value(bool expected) {
  constexpr bool actual = xstl::has_virtual_destructor<T>::value;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "has_virtual_destructor<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == expected,
    "has_virtual_destructor<T> returned incorrect value: actual = " << actual
    << ", expected = " << expected
    << ", T = " << typeid(T).name()
  );
#endif
}

struct HasVirtualDestructorUnitTest_NoVirtualDtor {
  ~HasVirtualDestructorUnitTest_NoVirtualDtor() {}
};

struct HasVirtualDestructorUnitTest_VirtualDtor {
  virtual ~HasVirtualDestructorUnitTest_VirtualDtor() {}
};

struct HasVirtualDestructorUnitTestDerived_VirtualDtor : HasVirtualDestructorUnitTest_VirtualDtor {};

struct HasVirtualDestructorUnitTest_NoDestructor {}; 

struct TestTypeInvokerHasVirtualDestructor {
  constexpr void operator()() const {
    tt_has_virtual_destructor_test_value<int>(false); 
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_NoVirtualDtor>(false);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_VirtualDtor>(true);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTestDerived_VirtualDtor>(true);
    tt_has_virtual_destructor_test_value<HasVirtualDestructorUnitTest_NoDestructor>(false);
  }
};

NOYX_TEST(HasVirtualDestructor, UnitTest) {
  TestTypeInvokerHasVirtualDestructor{}();
}
