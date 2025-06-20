#include <type_traits/add_cv.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, typename Expected>
constexpr void tt_add_cv_test_type() {
  static_assert(xstl::is_same_v<xstl::add_cv_t<T>, Expected>, "add_cv_t<T> returned wrong type");
  constexpr bool result = xstl::is_same_v<xstl::add_cv_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "add_cv_t<T> returned wrong type");
}

template <typename T>
struct TestTypeInvokerAddCv {
  constexpr void operator()() const {
    tt_add_cv_test_type<T, const volatile T>();
  }
};

NOYX_TEST(AddCv, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerAddCv>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerAddCv>(xstl_test_detail::all_test_types{});
}
