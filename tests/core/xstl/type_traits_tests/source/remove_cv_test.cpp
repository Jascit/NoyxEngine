#include <tests_details.hpp>
#include <tt_test_detail.hpp>
#include <is_same.hpp>   
#include <remove_cv.hpp>
#include <type_traits>
#include <iostream>

#define FLAG_TEST_WITH_MESSAGE 0

template<typename T>
constexpr void tt_remove_cv_test_type() {
  using RV = xstl::remove_cv<T>;
  using Clean = typename RV::type;

  bool result = xstl::is_same_v<Clean, std::remove_cv_t<T>>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "remove_cv::type does not match std::remove_cv_t");

#if FLAG_TEST_WITH_MESSAGE
  std::cout << "Testing remove_cv with " << typeid(T).name() << "\n";
#endif

  using Applied = typename RV::template apply<std::add_pointer>;

  //wtf i just wrote down? This shit just don't work, fuck this test
  using Expected = std::conditional_t<
    std::is_const_v<std::remove_reference_t<T>>&& std::is_volatile_v<std::remove_reference_t<T>>,
    const volatile std::add_pointer_t<std::remove_cv_t<T>>,
    std::conditional_t<
      std::is_const_v<std::remove_reference_t<T>>,
      const std::add_pointer_t<std::remove_cv_t<T>>,
      std::conditional_t<
        std::is_volatile_v<std::remove_reference_t<T>>,
        volatile std::add_pointer_t<std::remove_cv_t<T>>,
        std::add_pointer_t<std::remove_cv_t<T>>
        >
      >
    >;

  NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((std::is_same_v<Applied, Expected>)), "remove_cv::apply produces incorrect type");
}

template<typename T>
struct TestTypeInvoker {
  constexpr void operator()() const {
    tt_remove_cv_test_type<T>();
  }
};


template<std::size_t I = 0, typename Tuple>
void test_all_types(Tuple&& tuple) {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
    TestTypeInvoker<T>{}();
    test_all_types<I + 1>(std::forward<Tuple>(tuple));
  }
}

NOYX_TEST(RemoveCV, UnitTest) {
  std::cout << "\n----------TT_TESTS_REMOVE_CV----------\n";
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "\n----------ALL TYPES----------\n";
#endif
  test_all_types(xstl_test_detail::all_test_types{});
}

#undef FLAG_TEST_WITH_MESSAGE
