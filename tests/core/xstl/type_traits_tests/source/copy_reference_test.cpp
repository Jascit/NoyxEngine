#include <tt_test_detail.hpp>
#include <type_traits/copy_reference.hpp>

template<typename From, typename To, typename Expected>
constexpr void tt_copy_reference_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::copy_reference_t<From, To>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "copy_reference<" #From ", " #To "> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "copy_reference<" << typeid(From).name() << ", " << typeid(To).name()
    << "> returned incorrect: actual = "
    << typeid(xstl::copy_reference_t<From, To>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerCopyReference {
  void operator()() const {
    tt_copy_reference_test_value<int, double, double>();
    tt_copy_reference_test_value<int&, double, double&>();
    tt_copy_reference_test_value<int&&, double, double&&>();

    tt_copy_reference_test_value<int&, double&, double&>();
    tt_copy_reference_test_value<int&&, double&, double&>();
    tt_copy_reference_test_value<int&, double&&, double&>();
    tt_copy_reference_test_value<int&&, double&&, double&&>();

    // with cv qualifiers on To
    tt_copy_reference_test_value<int&, const float, const float&>();
    tt_copy_reference_test_value<int&&, volatile long, volatile long&&>();
    tt_copy_reference_test_value<int&, const volatile char&, const volatile char&>();
    tt_copy_reference_test_value<int&&, const volatile short&&, const volatile short&&>();
  }
};

NOYX_TEST(CopyReference, UnitTest) {
  TestTypeInvokerCopyReference{}();
}
