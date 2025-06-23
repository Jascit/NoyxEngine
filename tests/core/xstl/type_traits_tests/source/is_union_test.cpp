#include <type_traits/is_union.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, bool Expected>
constexpr void tt_is_union_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union<T>::value == Expected, "is_union returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union_v<T> == Expected, "is_union_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_union<T>::value == Expected, "is_union returned wrong value");
}

NOYX_TEST(IsUnion, UnitTest) {
  union MyUnion { int a; float b; };
  struct MyStruct {};
  class MyClass {};
  enum MyEnum { A, B, C };
  using FuncType = void();

  tt_is_union_test<MyUnion, true>();

  tt_is_union_test<MyStruct, false>();
  tt_is_union_test<MyClass, false>();
  tt_is_union_test<MyEnum, false>();
  tt_is_union_test<int, false>();
  tt_is_union_test<void, false>();
  tt_is_union_test<FuncType, false>();
  tt_is_union_test<int*, false>();
}
