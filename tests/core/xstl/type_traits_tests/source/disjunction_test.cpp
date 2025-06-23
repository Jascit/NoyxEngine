#include <type_traits/disjunction.hpp>
#include <type_traits/is_same.hpp>    
#include <tt_test_detail.hpp>          

template <bool Expected, typename... Bs>
constexpr void tt_disjunction_test_case() {
  constexpr bool result = xstl::disjunction<Bs...>::value;
  static_assert(result == Expected, "disjunction<Bs...>::value is incorrect");
  NOYX_ASSERT_TRUE_MESSAGE(result == Expected, "disjunction<Bs...> returned wrong result");
}
struct Disjunction_True1 { static constexpr bool value = true; };
struct Disjunction_True2 { static constexpr bool value = true; };
struct Disjunction_False1 { static constexpr bool value = false; };
struct Disjunction_False2 { static constexpr bool value = false; };
NOYX_TEST(Disjunction, UnitTest) {
  tt_disjunction_test_case<false>();
  tt_disjunction_test_case<true, Disjunction_True1>();
  tt_disjunction_test_case<false, Disjunction_False1>();
  tt_disjunction_test_case<true, Disjunction_True1, Disjunction_False1>();
  tt_disjunction_test_case<true, Disjunction_False1, Disjunction_True1>();
  tt_disjunction_test_case<false, Disjunction_False1, Disjunction_False2>();
  tt_disjunction_test_case<true, Disjunction_False1, Disjunction_False2, Disjunction_True2>();
  tt_disjunction_test_case<true, Disjunction_True1, Disjunction_True2, Disjunction_False1, Disjunction_False2>();
}
