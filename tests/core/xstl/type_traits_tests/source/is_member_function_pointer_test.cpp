#include <tt_test_detail.hpp>
#include <type_traits/is_member_function_pointer.hpp>

template<typename T, bool Expected>
constexpr void tt_is_member_function_pointer_test_value() {
  constexpr bool actual1 = xstl::is_member_function_pointer<T>::value;
  constexpr bool actual2 = xstl::is_member_function_pointer_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_member_function_pointer<" #T "> returned incorrect value"
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_member_function_pointer_v<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_member_function_pointer<" << typeid(T).name() << "> actual = "
    << actual1 << ", expected = " << Expected
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_member_function_pointer_v<" << typeid(T).name() << "> actual = "
    << actual2 << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsMemberFunctionPointer {
  constexpr void operator()() const {

    tt_is_member_function_pointer_test_value<void(), false>();
    tt_is_member_function_pointer_test_value<int(int, double), false>();

    tt_is_member_function_pointer_test_value<void(*)(int), false>();
    tt_is_member_function_pointer_test_value<int(*)(char), false>();

    struct D { int value; };
    tt_is_member_function_pointer_test_value<int D::*, false>();

    struct C {
      void foo();
      int bar(double) const;
      virtual void vf(int) {};
    };
    tt_is_member_function_pointer_test_value<decltype(&C::foo), true>();
    tt_is_member_function_pointer_test_value<decltype(&C::bar), true>();
    tt_is_member_function_pointer_test_value<decltype(&C::vf), true>();

    using MFP1 = void (C::*)() const;
    using MFP2 = int  (C::*)(double) volatile;
    tt_is_member_function_pointer_test_value<MFP1, true>();
    tt_is_member_function_pointer_test_value<MFP2, true>();

    tt_is_member_function_pointer_test_value<int[3], false>();
    tt_is_member_function_pointer_test_value<int*, false>();

    tt_is_member_function_pointer_test_value<int, false>();
    tt_is_member_function_pointer_test_value<void, false>();
  }
};

NOYX_TEST(IsMemberFunctionPointer, UnitTest) {
  TestTypeInvokerIsMemberFunctionPointer{}();
}
