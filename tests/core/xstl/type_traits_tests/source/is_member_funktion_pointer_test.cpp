#include <type_traits/is_member_function_pointer.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits/remove_reference.hpp>
#include <iostream>
#include <tt_test_detail.hpp>

struct TestClass
{
  void            foo();
  int             bar(int);
  double          baz(double, float) noexcept;
  void            variadic(...);
  static void     static_func();     
  int             data;             
};

// Helper-Funktion zum Einzeltest
template<typename T, bool Expected>
constexpr void tt_is_member_function_pointer_test_type()
{
  constexpr bool actual = xstl::is_member_function_pointer_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_member_function_pointer error, actual !== Expected"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_member_function_pointer<" << xstl::type_identity<T>::type() << "> returned "
    << actual << ", expected " << Expected
  );
#endif
}

template<typename T>
struct TestTypeInvokerIsMemberFunctionPointer
{
  constexpr void operator()() const
  {
    tt_is_member_function_pointer_test_type<T, false>();
  }
};

NOYX_TEST(IsMemberFunctionPointer, UnitTest)
{
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerIsMemberFunctionPointer>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerIsMemberFunctionPointer>(xstl_test_detail::all_test_types{});

  tt_is_member_function_pointer_test_type<void (TestClass::*)(), true>();
  tt_is_member_function_pointer_test_type<int  (TestClass::*)(int), true>();
  tt_is_member_function_pointer_test_type<double (TestClass::*)(double, float) noexcept, true>();
  tt_is_member_function_pointer_test_type<void (TestClass::*)(...), true>();

  tt_is_member_function_pointer_test_type<void (*)(), false>(); 
  tt_is_member_function_pointer_test_type<void(TestClass::*)() const, false>();
  tt_is_member_function_pointer_test_type<decltype(&TestClass::data), false>(); 
  tt_is_member_function_pointer_test_type<decltype(&TestClass::static_func), false>();
}
