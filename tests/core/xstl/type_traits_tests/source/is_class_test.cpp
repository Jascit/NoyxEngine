#include <type_traits/is_class.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

class IsClassUnitTestMyClass {};
struct IsClassUnitTestMyStruct {};
union IsClassUnitTestMyUnion {};
enum IsClassUnitTestMyEnum { A, B, C };
using IsClassUnitTestFuncType = void();

template <typename T, bool Expected>
constexpr void tt_is_class_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class<T>::value == Expected, "is_class returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class_v<T> == Expected, "is_class_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class<T>::value == Expected, "is_class returned wrong value");
}

NOYX_TEST(IsClass, UnitTest) {
  tt_is_class_test<IsClassUnitTestMyClass, true>();
  tt_is_class_test<IsClassUnitTestMyStruct, true>();

  tt_is_class_test<IsClassUnitTestMyUnion, false>();
  tt_is_class_test<IsClassUnitTestMyEnum, false>();
  tt_is_class_test<int, false>();
  tt_is_class_test<void, false>();
  tt_is_class_test<IsClassUnitTestFuncType, false>();
  tt_is_class_test<int*, false>();
}
