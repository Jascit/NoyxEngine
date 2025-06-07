#include <tests_details.hpp>
#include <type_identity.hpp>
#include <is/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits>
#include <iostream>
#define FLAG_TEST_WITH_MESSAGE 0

template<typename T>
constexpr void tt_type_identity_test_type() {
  using TI = xstl::type_identity<T>;
  bool result = xstl::is_same_v<typename TI::type, T>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "type_identity failed");
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "Testing type_identity with " << typeid(T).name() << "\n";
#endif
}

template<typename T>
struct TestTypeInvoker {
  constexpr void operator()() const {
    tt_type_identity_test_type<T>();
  }
};

template<std::size_t I = 0, typename Tuple>
void test_all_types_with_sufix(Tuple&& tuple) {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>){
    using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
    xstl_test_detail::for_each_type<xstl_test_detail::all_suffixes<T>, TestTypeInvoker>();
    test_all_types_with_sufix<I+1>(std::forward<Tuple>(tuple));
  }
}

NOYX_TEST(TypeIdentity, UnitTest) {

  std::cout << "\n----------TT_TESTS_TYPE_IDENTITY----------\n";
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL TYPES----------\n";
#endif
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvoker>();
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL SUFIXES----------\n";
#endif
  test_all_types_with_sufix(xstl_test_detail::all_test_types{});
}

#undef FLAG_TEST_WITH_MESSAGE
