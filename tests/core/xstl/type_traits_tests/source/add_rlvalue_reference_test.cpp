#include <is_same.hpp>
#include <tt_test_detail.hpp>
#include <add_lvalue_reference.hpp> 
#include <add_rvalue_reference.hpp>
#include <tuple>
#include <iostream>

template<typename T>
constexpr void tt_add_lvalue_ref_test_type() {
#if TEST_WITH_STATIC_ASSERT

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same<typename xstl::add_lvalue_reference<T>::type, T&>::value)),
    "add_lvalue_reference failed"
  );

#else

  using L = xstl::add_lvalue_reference<T>;
  bool result = xstl::is_same_v<typename L::type, T&>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "add_lvalue_reference failed for type " << typeid(T).name());

#endif
}

template<typename T>
struct TestLValueInvoker {
  constexpr void operator()() const {
    tt_add_lvalue_ref_test_type<T>();
  }
};

template<typename T>
constexpr void tt_add_rvalue_ref_test_type() {
#if TEST_WITH_STATIC_ASSERT

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same<typename xstl::add_rvalue_reference<T>::type, T&&>::value)),
    "add_rvalue_reference failed"
  );

#else

  using R = xstl::add_rvalue_reference<T>;
  bool result = xstl::is_same_v<typename R::type, T&&>;
  NOYX_ASSERT_TRUE_MESSAGE(result, "add_rvalue_reference failed for type " << typeid(T).name());

#endif
}
template<typename T>
struct TestRValueInvoker {
  constexpr void operator()() const {
    tt_add_rvalue_ref_test_type<T>();
  }
};

template<std::size_t I = 0, template<typename> typename Invoker, typename Tuple>
void test_all_types_with_suffix(Tuple&& tuple) {
  if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
    xstl_test_detail::for_each_type<xstl_test_detail::all_suffixes<T>, Invoker>();
    test_all_types_with_suffix<I + 1, Invoker>(std::forward<Tuple>(tuple));
  }
}

NOYX_TEST(AddLValueReference, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestLValueInvoker>();
  xstl_test_detail::test_all_types_with_suffix<0, TestLValueInvoker>(xstl_test_detail::all_test_types{});
}

NOYX_TEST(AddRValueReference, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestRValueInvoker>();
  xstl_test_detail::test_all_types_with_suffix<0, TestRValueInvoker>(xstl_test_detail::all_test_types{});
}

#undef FLAG_TEST_WITH_MESSAGE
