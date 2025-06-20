#include <type_traits/is_union.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

union IsUnionUnitTestMyUnion { int a; float b; };
struct IsUnionUnitTestMyStruct {};
class IsUnionUnitTestMyClass {};
enum IsUnionUnitTestMyEnum { A, B, C };
using IsUnionUnitTestFuncType = void();

template <typename T, bool Expected>
constexpr void tt_is_union_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union<T>::value == Expected, "is_union returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union_v<T> == Expected, "is_union_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union<T>::value == Expected, "is_union returned wrong value");
}

NOYX_TEST(IsUnion, UnitTest) {
  tt_is_union_test<IsUnionUnitTestMyUnion, true>();

  tt_is_union_test<IsUnionUnitTestMyStruct, false>();
  tt_is_union_test<IsUnionUnitTestMyClass, false>();
  tt_is_union_test<IsUnionUnitTestMyEnum, false>();
  tt_is_union_test<int, false>();
  tt_is_union_test<void, false>();
  tt_is_union_test<IsUnionUnitTestFuncType, false>();
  tt_is_union_test<int*, false>();
}
