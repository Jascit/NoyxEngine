#include <type_traits/rank.hpp>               
#include <type_traits/is_same.hpp>
#include <type_traits/remove_reference.hpp>
#include <type_traits/remove_all_extents.hpp>
#include <type_traits/conditional.hpp>
#include <type_traits>
#include <iostream>
#include <tt_test_detail.hpp>

template<typename T, xstl::size_t ExpectedRank>
constexpr void tt_rank_test_type() {
  constexpr xstl::size_t actual = xstl::rank_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == ExpectedRank)),
    "rank<T> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == ExpectedRank)),
    "rank<T> returned incorrect value: got " << actual << ", expected " << ExpectedRank
  );
#endif // TEST_WITH_STATIC_ASSERT

}

template<typename T>
struct TestTypeInvokerRank {
  constexpr void operator()() const {
    using Tr = xstl::remove_reference_t<T>;
    using Tre = xstl::remove_all_extents_t<Tr>;
    using Tref = xstl::conditional_t<std::is_function_v<Tre>, int, Tre>;
    tt_rank_test_type<Tref, 0>();        
    tt_rank_test_type<Tref[1], 1>();     
    tt_rank_test_type<Tref[1][2], 2>();  
    tt_rank_test_type<Tref[][3], 2>();   
    tt_rank_test_type<Tref[4][3][2], 3>(); 
  }
};

NOYX_TEST(Rank, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerRank>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerRank>(xstl_test_detail::all_test_types{});
}