#include <type_traits/is_class.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>


template <typename T, bool Expected>
constexpr void tt_is_class_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class<T>::value == Expected, "is_class returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class_v<T> == Expected, "is_class_v returned wrong value");
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_class<T>::value == Expected, "is_class returned wrong value");
}

NOYX_TEST(IsClass, UnitTest) {
  class MyClass {};
  struct MyStruct {};
  union MyUnion {};
  enum MyEnum { A, B, C };
  using FuncType = void();

  tt_is_class_test<MyClass, true>();
  tt_is_class_test<MyStruct, true>();

  tt_is_class_test<MyUnion, false>();
  tt_is_class_test<MyEnum, false>();
  tt_is_class_test<int, false>();
  tt_is_class_test<void, false>();
  tt_is_class_test<FuncType, false>();
  tt_is_class_test<int*, false>();
}
