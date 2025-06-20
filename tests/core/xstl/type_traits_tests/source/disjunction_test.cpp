#include <type_traits/disjunction.hpp>
#include <type_traits/is_same.hpp>    
#include <tt_test_detail.hpp>          

template <bool Expected, typename... Bs>
constexpr void tt_disjunction_test_case() {
  constexpr bool result = xstl::disjunction<Bs...>::value;
  static_assert(result == Expected, "disjunction<Bs...>::value is incorrect");
  NOYX_ASSERT_TRUE_MESSAGE(result == Expected, "disjunction<Bs...> returned wrong result");
}

struct True1 { static constexpr bool value = true; };
struct True2 { static constexpr bool value = true; };
struct False1 { static constexpr bool value = false; };
struct False2 { static constexpr bool value = false; };

NOYX_TEST(Disjunction, UnitTest) {
  tt_disjunction_test_case<false>();                                    
  tt_disjunction_test_case<true, True1>();                             
  tt_disjunction_test_case<false, False1>();                            
  tt_disjunction_test_case<true, True1, False1>();                     
  tt_disjunction_test_case<true, False1, True1>();                     
  tt_disjunction_test_case<false, False1, False2>();                    
  tt_disjunction_test_case<true, False1, False2, True2>();             
  tt_disjunction_test_case<true, True1, True2, False1, False2>();
}
