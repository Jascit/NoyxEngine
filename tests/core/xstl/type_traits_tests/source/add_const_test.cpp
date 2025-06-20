#include <type_traits/add_const.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, typename Expected>
constexpr void tt_add_const_test_type() {
  static_assert(xstl::is_same_v<xstl::add_const_t<T>, Expected>, "add_const_t<T> returned wrong type");
  constexpr bool result = xstl::is_same_v<xstl::add_const_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "add_const_t<T> returned wrong type");
}

template <typename T>
struct TestTypeInvokerAddConst {
  constexpr void operator()() const {
    tt_add_const_test_type<T, const T>();
  }
};

NOYX_TEST(AddConst, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerAddConst>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerAddConst>(xstl_test_detail::all_test_types{});
}
