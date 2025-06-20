#include <type_traits/is_same.hpp>  
#include <type_traits/add_volatile.hpp> 
#include <tt_test_detail.hpp>           

template <typename T, typename Expected>
constexpr void tt_add_volatile_test_type() {
  static_assert(xstl::is_same_v<xstl::add_volatile_t<T>, Expected>, "add_volatile_t<T> returned wrong type");
  constexpr bool result = xstl::is_same_v<xstl::add_volatile_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(
    (result),
    "add_volatile_t<T> returned wrong type"
  );
}

template <typename T>
struct TestTypeInvokerAddVolatile {
  constexpr void operator()() const {
    tt_add_volatile_test_type<T, volatile T>();
  }
};

NOYX_TEST(AddVolatile, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerAddVolatile>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerAddVolatile>(xstl_test_detail::all_test_types{});
}
