#include <type_traits/negation.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

struct Negation_TrueType { static constexpr bool value = true; };
struct Negation_FalseType { static constexpr bool value = false; };

template <typename B, bool Expected>
constexpr void tt_negation_test_type() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::negation<B>::value == Expected, "negation returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::negation_v<B> == Expected, "negation_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::negation<B>::value == Expected, "negation returned wrong value");
}

NOYX_TEST(Negation, UnitTest) {
  tt_negation_test_type<Negation_TrueType, false>();
  tt_negation_test_type<Negation_FalseType, true>();
}
