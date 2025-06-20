#include <type_traits/is_same.hpp> 
#include <type_traits/remove_all_extents.hpp> 
#include <type_traits/is_reference.hpp>
#include <type_traits/is_function.hpp>
#include <type_traits/is_abstract.hpp>
#include <tt_test_detail.hpp>             

template <typename T, typename Expected>
constexpr void tt_remove_all_extents_test_type() {
  constexpr bool result = xstl::is_same_v<xstl::remove_all_extents_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(
    (result),
    "remove_all_extents_t<T> returned wrong type"
  );
}

template <typename T>
struct TestTypeInvokerRemoveAllExtents {
  constexpr void operator()() const {
    tt_remove_all_extents_test_type<T, T>();

    if constexpr (
      !xstl::is_reference_v<T> &&
      !xstl::is_function_v<T> &&
      !xstl::is_abstract_v<T>
      ) {
      tt_remove_all_extents_test_type<T[3], T>();
      tt_remove_all_extents_test_type<T[3][4], T>();
      tt_remove_all_extents_test_type<T[1][4], T>();
      tt_remove_all_extents_test_type<T[5][6][7], T>();
    }
  }
};

NOYX_TEST(RemoveAllExtents, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerRemoveAllExtents>();
}
