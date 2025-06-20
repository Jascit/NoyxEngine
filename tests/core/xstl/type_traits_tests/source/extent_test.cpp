#include <type_traits/extent.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, std::size_t I, std::size_t Expected>
constexpr void tt_extent_test() {
  constexpr std::size_t val = xstl::extent<T, I>::value;
  static_assert(val == Expected, "extent returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(val == Expected, "extent returned wrong value");
}

NOYX_TEST(Extent, UnitTest) {
  tt_extent_test<int, 0, 0>();         
  tt_extent_test<int[], 0, 0>();              
  tt_extent_test<int[3], 0, 3>();              
  tt_extent_test<int[3][4], 0, 3>();          
  tt_extent_test<int[3][4], 1, 4>();           
  tt_extent_test<int[3][4][5], 0, 3>();        
  tt_extent_test<int[3][4][5], 1, 4>();        
  tt_extent_test<int[3][4][5], 2, 5>();        
}
