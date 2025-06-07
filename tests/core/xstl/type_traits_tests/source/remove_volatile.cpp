#include <tests_details.hpp>
#include <tt_test_detail.hpp>
#include <remove_volatile.hpp>
#include <is_same.hpp>
#include <iostream>

#define FLAG_TEST_WITH_MESSAGE 1

template<typename T>
constexpr void tt_remove_volatile_test_type() {
  using Removed = xstl::remove_volatile_t<T>;
  using Expected = typename std::remove_volatile<T>::type; // f?r Vergleich mit std

  bool result = xstl::is_same_v<Removed, Expected>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "remove_volatile failed");

#if FLAG_TEST_WITH_MESSAGE
  std::cout << "Testing remove_volatile with " << typeid(T).name() << "\n";
#endif
}

template<typename T>
struct TestTypeInvoker {
  constexpr void operator()() const {
    tt_remove_volatile_test_type<T>();
  }
};

template<std::size_t I = 0, typename Tuple>
void test_all_types_with_suffix(Tuple&& tuple) {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
    xstl_test_detail::for_each_type<xstl_test_detail::all_suffixes<T>, TestTypeInvoker>();
    test_all_types_with_suffix<I + 1>(std::forward<Tuple>(tuple));
  }
}

NOYX_TEST(RemoveVolatile, UnitTest) {
  std::cout << "\n----------TT_TESTS_REMOVE_VOLATILE----------\n";
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL TYPES----------\n";
#endif
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvoker>();
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL SUFFIXES----------\n";
#endif
  test_all_types_with_suffix(xstl_test_detail::all_test_types{});
}

#undef FLAG_TEST_WITH_MESSAGE