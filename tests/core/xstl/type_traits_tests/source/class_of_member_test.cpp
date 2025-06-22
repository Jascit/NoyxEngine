#include <tt_test_detail.hpp>
#include <type_traits/class_of_member.hpp>

template<typename MemberPtr, typename ExpectedClass>
constexpr void tt_class_of_member_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::class_of_member_t<MemberPtr>, ExpectedClass>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "class_of_member<" #MemberPtr "> returned incorrect class"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "class_of_member<" << typeid(MemberPtr).name() << "> returned incorrect class: "
    "actual = " << typeid(xstl::class_of_member_t<MemberPtr>).name()
    << ", expected = " << typeid(ExpectedClass).name()
  );
#endif
}

struct TestTypeInvokerClassOfMember {
  struct A {
    int data;
    double func(int);
    double cfunc(int) const;
    double ncfunc(int) noexcept;
    double ccfunc(int) const noexcept;
    double rreffunc(int)&;
    double xvaluefunc(int)&&;
  };
  struct B {};
  struct Outer { struct Inner { char c; }; };

  void operator()() const {
    tt_class_of_member_test_value<int A::*, A>();
    tt_class_of_member_test_value<const int A::*, A>();
    tt_class_of_member_test_value<volatile int A::*, A>();
    tt_class_of_member_test_value<const volatile int A::*, A>();
    tt_class_of_member_test_value<double (A::*)(int), A>();
    tt_class_of_member_test_value<double (A::*)(int) const, A>();
    tt_class_of_member_test_value<double (A::*)(int) noexcept, A>();
    tt_class_of_member_test_value<double (A::*)(int) const noexcept, A>();
    tt_class_of_member_test_value<double (A::*)(int)&, A>();
    tt_class_of_member_test_value<double (A::*)(int)&&, A>();
    tt_class_of_member_test_value<char Outer::Inner::*, Outer::Inner>();
  }
};

NOYX_TEST(ClassOfMember, UnitTest) {
  TestTypeInvokerClassOfMember{}();
}
