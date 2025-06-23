#include <type_traits/remove_const.hpp>       // xstl::remove_const, remove_const_t
#include <type_traits/is_same.hpp>            // xstl::is_same_v
#include <tt_test_detail.hpp>     // all_suffixes, for_each_type
#include <type_traits>
#include <iostream>

template<typename T>
constexpr void test_remove_const() {
  using Removed = typename xstl::remove_const<T>::type;
  using RemovedAlias = xstl::remove_const_t<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<Removed, std::remove_const_t<T>>)), "xstl::remove_const<T>::type failed ");
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<RemovedAlias, std::remove_const_t<T>>)), "xstl::remove_const_t<T> failed");
#else
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<Removed, std::remove_const_t<T>>)), std::string("xstl::remove_const<T>::type failed for type: " ) << typeid(T).name());
  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same_v<RemovedAlias, std::remove_const_t<T>>)), std::string("xstl::remove_const_t<T> failed for type: " ) << typeid(T).name());
#endif
}


template<typename T>
struct RemoveConstInvoker {
  constexpr void operator()() const {
    test_remove_const<T>();
  }
};

NOYX_TEST(RemoveConst, UnitTest) {
  xstl_test_detail::test_all_types_with_suffix<0, RemoveConstInvoker>(xstl_test_detail::all_test_types{});
}
