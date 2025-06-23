#include <tt_test_detail.hpp>
#include <type_traits>
#include <type_traits/copy_cv.hpp>

template<typename From, typename To, typename Expected>
constexpr void tt_copy_cv_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::copy_cv_t<From, To>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "copy_cv<" #From ", " #To "> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "copy_cv<" << typeid(From).name() << ", " << typeid(To).name()
    << "> returned incorrect: actual = "
    << typeid(xstl::copy_cv_t<From, To>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerCopyCV {
  void operator()() const {
    tt_copy_cv_test_value<int, double, double>();

    tt_copy_cv_test_value<const int, double, const double>();

    tt_copy_cv_test_value<volatile int, double, volatile double>();

    tt_copy_cv_test_value<const volatile int, double, const volatile double>();

    tt_copy_cv_test_value<const int&, double, double>();
    tt_copy_cv_test_value<volatile long&, int, int>();

    tt_copy_cv_test_value<const char*, float, float>();
    tt_copy_cv_test_value<volatile short*, long, long>();
    tt_copy_cv_test_value<const volatile bool* const , bool, const bool>();

    tt_copy_cv_test_value<const volatile bool*, volatile bool, volatile bool>();
    tt_copy_cv_test_value<const volatile bool*, const bool, const bool>();
    tt_copy_cv_test_value<const volatile bool*, const volatile bool, const volatile bool>();
    tt_copy_cv_test_value<const volatile bool*, const volatile bool*, const volatile bool*>();
  }
};

NOYX_TEST(CopyCV, UnitTest) {
  TestTypeInvokerCopyCV{}();
}

