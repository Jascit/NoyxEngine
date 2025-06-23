#include <tt_test_detail.hpp>
#include <type_traits/copy_cv_ref.hpp>

template<typename From, typename To, typename Expected>
constexpr void tt_copy_cv_ref_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::copy_cv_ref_t<From, To>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "copy_cv_ref<" #To ", " #From "> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "copy_cv_ref<" << typeid(From).name() << ", " << typeid(To).name()
    << "> returned incorrect: actual = "
    << typeid(xstl::copy_cv_ref_t<To, From>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerCopyCVRef {
  void operator()() const {
    tt_copy_cv_ref_test_value<const char, int, const int>();
    tt_copy_cv_ref_test_value<volatile char, int, volatile int>();
    tt_copy_cv_ref_test_value<const volatile char, int, const volatile int>();
    tt_copy_cv_ref_test_value<char&, int, int&>();
    tt_copy_cv_ref_test_value<const char&, int, const int&>();
    tt_copy_cv_ref_test_value<volatile char&, int, volatile int&>();
    tt_copy_cv_ref_test_value<const volatile char&, int, const volatile int&>();
    tt_copy_cv_ref_test_value<char&&, int, int&&>();
    tt_copy_cv_ref_test_value<const char&&, int, const int&&>();
    tt_copy_cv_ref_test_value<volatile char&&, int, volatile int&&>();
    tt_copy_cv_ref_test_value<const volatile char&&, int, const volatile int&&>();
    tt_copy_cv_ref_test_value<char&, int&&, int&>();
    tt_copy_cv_ref_test_value<const char, int&, const int>(); // this shit doesn't work
    tt_copy_cv_ref_test_value<volatile char&, int&, volatile int&>(); // this shit doesn't work
    tt_copy_cv_ref_test_value<const volatile char&&, int&, const volatile int&&>(); // this shit doesn't work
  }
};

NOYX_TEST(CopyCVRef, UnitTest) {
  TestTypeInvokerCopyCVRef{}();
}