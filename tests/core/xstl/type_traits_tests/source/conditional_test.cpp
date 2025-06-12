#include <type_traits>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>
#include <type_traits/conditional.hpp>

template<bool Cond, typename TTrue, typename TFalse>
constexpr void test_conditional_type() {
  using expected = std::conditional_t<Cond, TTrue, TFalse>;
  using result = xstl::conditional_t<Cond, TTrue, TFalse>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<expected, result>)),
    "xstl::conditional_t returns an incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<expected, result>)),
    "xstl::conditional_t<" << (Cond ? "true" : "false") << ", T, F> returns an incorrect type"
  );
#endif
}

template<typename T>
struct TestTypeInvokerConditional {
  constexpr void operator()() const {
    test_conditional_type<true, T, int>();
    test_conditional_type<false, int, T>();
  }
};

NOYX_TEST(Conditional, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerConditional>();
}
