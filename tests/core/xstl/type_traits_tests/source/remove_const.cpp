#include <remove_const.hpp>       // xstl::remove_const, remove_const_t
#include <is/is_same.hpp>            // xstl::is_same_v
#include <tests_details.hpp>      // NOYX_ASSERT_TRUE_MESSAGE
#include <tt_test_detail.hpp>     // all_suffixes, for_each_type
#include <type_traits>
#include <iostream>

#define FLAG_TEST_WITH_MESSAGE 0

template<typename T>
constexpr void test_remove_const() {
  using Removed = typename xstl::remove_const<T>::type;
  using RemovedAlias = xstl::remove_const_t<T>;

#if FLAG_TEST_WITH_MESSAGE
  std::cout << "Testing remove_const with " << typeid(T).name() << "\n";
#endif

  bool result1 = xstl::is_same_v<Removed, std::remove_const_t<T>>;
  bool result2 = xstl::is_same_v<RemovedAlias, std::remove_const_t<T>>;

  NOYX_ASSERT_TRUE_MESSAGE(result1,
    std::string("xstl::remove_const<T>::type failed for type: ") + typeid(T).name());

  NOYX_ASSERT_TRUE_MESSAGE(result2,
    std::string("xstl::remove_const_t<T> failed for type: ") + typeid(T).name());
}


template<typename T>
struct RemoveConstInvoker {
  constexpr void operator()() const {
    test_remove_const<T>();
  }
};

template<std::size_t I = 0, typename Tuple>
void remove_const_test_all_suffixes() {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using Base = std::tuple_element_t<I, Tuple>;
    using Suffixes = xstl_test_detail::all_suffixes<Base>;
    xstl_test_detail::for_each_type<Suffixes, RemoveConstInvoker>();
    remove_const_test_all_suffixes<I + 1, Tuple>();
  }
}

NOYX_TEST(RemoveConst, UnitTest) {
  std::cout << "\n----------TT_TESTS_REMOVE_CONST----------\n";
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL SUFFIXES----------\n";
#endif
  remove_const_test_all_suffixes<0, xstl_test_detail::all_test_types>();
}