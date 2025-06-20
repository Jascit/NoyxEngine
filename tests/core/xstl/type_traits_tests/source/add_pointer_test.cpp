#include <type_traits/add_pointer.hpp>
#include <type_traits/is_same.hpp>    
#include <type_traits/is_reference.hpp>
#include <type_traits/is_function.hpp>
#include <tt_test_detail.hpp>          

template <typename T, typename Expected>
constexpr void tt_add_pointer_test_type() {
  static_assert(xstl::is_same_v<xstl::add_pointer_t<T>, Expected>,
    "add_pointer_t<T> returned wrong type");
  constexpr bool result = xstl::is_same_v<xstl::add_pointer_t<T>, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "add_pointer_t<T> returned wrong type");
}

template <typename T>
struct TestTypeInvokerAddPointer {
  constexpr void operator()() const {
    if constexpr (xstl::is_reference_v<T> || xstl::is_function_v<T>) {
      tt_add_pointer_test_type<T, T>();
    }
    else {
      tt_add_pointer_test_type<T, T*>();
    }
  }
};

NOYX_TEST(AddPointer, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerAddPointer>();
}
