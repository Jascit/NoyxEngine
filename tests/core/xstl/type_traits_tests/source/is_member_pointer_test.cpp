#include <tt_test_detail.hpp>
#include <type_traits/is_member_pointer.hpp>

template<typename T, bool Expected>
constexpr void tt_is_member_pointer_test_value() {
  constexpr bool actual1 = xstl::is_member_pointer<T>::value;
  constexpr bool actual2 = xstl::is_member_pointer_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_member_pointer<" #T "> returned incorrect value"
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_member_pointer_v<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_member_pointer<" << typeid(T).name()
    << "> actual = " << actual1 << ", expected = " << Expected
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_member_pointer_v<" << typeid(T).name()
    << "> actual = " << actual2 << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsMemberPointer {
  constexpr void operator()() const {
    tt_is_member_pointer_test_value<int*, false>();
    tt_is_member_pointer_test_value<void*, false>();

    struct S { int a; double b; };
    tt_is_member_pointer_test_value<int S::*, true>();
    tt_is_member_pointer_test_value<double S::*, true>();

    struct C { void foo(); int bar(double) const; };
    tt_is_member_pointer_test_value<decltype(&C::foo), true>();
    tt_is_member_pointer_test_value<decltype(&C::bar), true>();

    struct Q { const int x; volatile char y; };
    tt_is_member_pointer_test_value<const int Q::*, true>();
    tt_is_member_pointer_test_value<volatile char Q::*, true>();

    struct A { int arr[5]; };
    tt_is_member_pointer_test_value<decltype(&A::arr), true>();

    tt_is_member_pointer_test_value<void(), false>();
    tt_is_member_pointer_test_value<int[3], false>();

    tt_is_member_pointer_test_value<int, false>();
    tt_is_member_pointer_test_value<void, false>();
  }
};

NOYX_TEST(IsMemberPointer, UnitTest) {
  TestTypeInvokerIsMemberPointer{}();
}
