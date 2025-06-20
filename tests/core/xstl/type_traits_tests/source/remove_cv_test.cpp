#include <tt_test_detail.hpp>
#include <type_traits/remove_cv.hpp>
#include <type_traits/is_same.hpp>

template <typename T, typename Expected>
constexpr void tt_remove_cv_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::remove_cv_t<T>, Expected>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "remove_cv<T> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "remove_cv<" << typeid(T).name() << "> returned incorrect type: "
    << "actual = " << typeid(xstl::remove_cv_t<T>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerRemoveCV {
  constexpr void operator()() const {
    tt_remove_cv_test_value<int, int>();
    tt_remove_cv_test_value<const int, int>();
    tt_remove_cv_test_value<volatile int, int>();
    tt_remove_cv_test_value<const volatile int, int>();

    tt_remove_cv_test_value<int[], int[]>();
    tt_remove_cv_test_value<const int[], int[]>();
    tt_remove_cv_test_value<volatile int[], int[]>();
    tt_remove_cv_test_value<const volatile int[], int[]>();

    tt_remove_cv_test_value<int[3], int[3]>();
    tt_remove_cv_test_value<const int[3], int[3]>();
    tt_remove_cv_test_value<volatile int[3], int[3]>();
    tt_remove_cv_test_value<const volatile int[3], int[3]>();

    tt_remove_cv_test_value<const int*, const int*>();
    tt_remove_cv_test_value<volatile int*, volatile int*>();
    tt_remove_cv_test_value<const volatile int*, const volatile int*>();
  }
};

NOYX_TEST(RemoveCV, UnitTest) {
  TestTypeInvokerRemoveCV{}();
}
