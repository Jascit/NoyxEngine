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


struct TestTypeInvokerHasVirtualDestructor {
  constexpr void operator()() const {
  struct NoVirtualDtor {
    ~NoVirtualDtor() {}
  };
  struct VirtualDtor {
    virtual ~VirtualDtor() {}
  };
  struct Derived_VirtualDtor : VirtualDtor {};
  struct NoDestructor {}; 

    tt_has_virtual_destructor_test_value<int>(false); 
    tt_has_virtual_destructor_test_value<NoVirtualDtor>(false);
    tt_has_virtual_destructor_test_value<VirtualDtor>(true);
    tt_has_virtual_destructor_test_value<Derived_VirtualDtor>(true);
    tt_has_virtual_destructor_test_value<NoDestructor>(false);
  }
};

NOYX_TEST(HasVirtualDestructor, UnitTest) {
  TestTypeInvokerHasVirtualDestructor{}();
}
