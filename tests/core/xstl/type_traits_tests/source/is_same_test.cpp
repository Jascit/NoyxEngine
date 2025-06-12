#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits>

template<typename T>
constexpr void tt_test_is_same_two_T() {
#if TEST_WITH_STATIC_ASSERT
  static_assert(xstl::is_same<T, T>::value, "xstl::is_same<T, T>::value was expected to be true");
  static_assert(xstl::is_same_v<T, T>, "xstl::is_same_v<T, T> was expected to be true");
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same<T, T>::value)),
    "xstl::is_same<T, T>::value was expected to be true for type: " << typeid(T).name()
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<T, T>)),
    "xstl::is_same_v<T, T> was expected to be true for type: " << typeid(T).name()
  );
#endif
}

template<typename T>
class TestIsSameInvoker {
public:
  constexpr void operator()() {
    tt_test_is_same_two_T<T>();
  }
};

template<std::size_t I = 0, typename Tuple>
void is_same_test_all_sufixes() {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using TI = std::tuple_element_t<I, Tuple>;
    using Sufixes = xstl_test_detail::all_suffixes<TI>;
    xstl_test_detail::for_each_type<Sufixes, TestIsSameInvoker>();
    is_same_test_all_sufixes<I + 1, Tuple>();
  }
}

template<typename T, typename U>
constexpr void tt_test_is_same_different() {
#if TEST_WITH_STATIC_ASSERT

  NOYX_ASSERT_FALSE_MESSAGE(
    NOYX_EVAL((xstl::is_same<T, U>::value)),
    "xstl::is_same<T, U>::value was expected to be false"
  );

  NOYX_ASSERT_FALSE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<T, U>)),
    "xstl::is_same_v<T, U> was expected to be false"
  );

#else

  bool result = xstl::is_same<T, U>::value;
  bool result_v = xstl::is_same_v<T, U>;

  NOYX_ASSERT_FALSE_MESSAGE(result,
    std::string("xstl::is_same<T, U>::value was expected to be false, but got true. Types: ") +
    typeid(T).name() + " vs " + typeid(U).name());

  NOYX_ASSERT_FALSE_MESSAGE(result_v,
    std::string("xstl::is_same_v<T, U> was expected to be false, but got true. Types: ") +
    typeid(T).name() + " vs " + typeid(U).name());

#endif
}

template<typename T, typename Tuple, std::size_t I = 0, std::size_t Y = 0>
void test_different_from_all() {
  if constexpr (I < std::tuple_size_v<Tuple>) {
    using U = std::tuple_element_t<I, Tuple>;
    if constexpr (I != Y) {
      tt_test_is_same_different<T, U>();
    }
    test_different_from_all<T, Tuple, I + 1, Y>();
  }
}


template<typename Tuple, std::size_t I = 0>
void test_all_different_combinations() {
  if constexpr (I < std::tuple_size_v<Tuple>) {
    using T = std::tuple_element_t<I, Tuple>;
    test_different_from_all<T, Tuple, 0, I>();
    test_all_different_combinations<Tuple, I + 1>();
  }
}

NOYX_TEST(IsSame, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestIsSameInvoker>();
  is_same_test_all_sufixes<0, xstl_test_detail::all_test_types>();
  test_all_different_combinations<xstl_test_detail::all_test_types>();

}
