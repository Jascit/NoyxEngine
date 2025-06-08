#include <is_same.hpp>
#include <tt_test_detail.hpp>
#include <remove_cv.hpp>  
#include <type_traits>               
#include <tuple>
#include <iostream>

template <typename U>
struct Dummy {};
template<typename T>
constexpr void tt_remove_cv_type_test() {
#if TEST_WITH_STATIC_ASSERT

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same<typename xstl::remove_cv<T>::type, typename std::remove_cv<T>::type>::value)),
    "remove_cv::type failed"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same<xstl::remove_cv_t<T>, std::remove_cv_t<T>>::value)),
    "remove_cv_t failed"
  );

#else

  bool res1 = xstl::is_same_v<typename xstl::remove_cv<T>::type, typename std::remove_cv<T>::type>;
  NOYX_ASSERT_TRUE_MESSAGE(res1, "remove_cv::type failed for " << typeid(T).name());

  bool res2 = xstl::is_same_v<xstl::remove_cv_t<T>, std::remove_cv_t<T>>;
  NOYX_ASSERT_TRUE_MESSAGE(res2, "remove_cv_t failed for " << typeid(T).name());

#endif
}

template<typename T>
constexpr void tt_remove_cv_apply_test() {
  using My = xstl::remove_cv<T>;
  using A = typename My::template apply<Dummy>;
  using Base = std::remove_cv_t<T>;
  using Raw = Dummy<Base>;

  using WithC = std::conditional_t<std::is_const_v<T>,
    std::add_const_t<Raw>,
    Raw>;
  using WithCV = std::conditional_t<std::is_volatile_v<T>,
    std::add_volatile_t<WithC>,
    WithC>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<A, WithCV>)),
    "remove_cv::apply<Dummy> failed"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<A, WithCV>)),
    "remove_cv::apply<Dummy> failed: expected " << typeid(WithCV).name() << ", got " << typeid(A).name()
  );
#endif
}

template<typename T>
struct TestRemoveCvInvoker {
  constexpr void operator()() const {
    tt_remove_cv_type_test<T>();
    tt_remove_cv_apply_test<T>();
  }
};
template<std::size_t I = 0, typename Tuple>
void test_all_with_suffix(Tuple&& tuple) {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
    xstl_test_detail::for_each_type<xstl_test_detail::all_suffixes<T>, TestRemoveCvInvoker>();
    test_all_with_suffix<I + 1>(std::forward<Tuple>(tuple));
  }
}

NOYX_TEST(RemoveCV, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestRemoveCvInvoker>();
  xstl_test_detail::test_all_types_with_suffix<0, TestRemoveCvInvoker>(xstl_test_detail::all_test_types{});
}
