#include <type_traits/is_same.hpp>
#include <type_traits/remove_cv.hpp> 
#include <tt_test_detail.hpp>
#include <tuple>
#include <iostream>

template <typename T>
struct Dummy {
  using type = T;
};

template <typename T>
constexpr void tt_preserve_cv_test_type() {
  using Preserved = xstl::preserve_cv_t<T, Dummy>;
  using Raw = Dummy<std::remove_cv_t<T>>;

  using WithConst = std::conditional_t<std::is_const_v<T>, std::add_const_t<Raw>, Raw>;
  using WithCV = std::conditional_t<std::is_volatile_v<T>, std::add_volatile_t<WithConst>, WithConst>;

#if TEST_WITH_STATIC_ASSERT
  static_assert(xstl::is_same_v<Preserved, WithCV>, "preserve_cv::type failed for Preserved");
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<Preserved, WithCV>)),
    "preserve_cv::type failed for Preserved: " << typeid(Preserved).name() << " Expected: " << typeid(WithCV).name()
  );
#endif

}


template <typename T>
struct TestPreserveCVInvoker {
  constexpr void operator()() const {
    tt_preserve_cv_test_type<T>();
  }
};

NOYX_TEST(PreserveCV, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestPreserveCVInvoker>();
  xstl_test_detail::test_all_types_with_suffix<0, TestPreserveCVInvoker>(xstl_test_detail::all_test_types{});
}
