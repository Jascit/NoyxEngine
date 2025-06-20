//#include <type_traits/is_member_object_pointer.hpp>
//#include <type_traits/is_member_function_pointer.hpp>
//#include <tt_test_detail.hpp>
//
//struct A {
//  int data;
//  void func();
//  int x;
//};
//
//template <typename T, bool Expected>
//constexpr void tt_is_member_object_pointer_test() {
//  static_assert(xstl::is_member_object_pointer<T>::value == Expected, "is_member_object_pointer returned wrong value");
//  static_assert(xstl::is_member_object_pointer_v<T> == Expected, "is_member_object_pointer_v returned wrong value");
//  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_member_object_pointer<T>::value == Expected, "is_member_object_pointer returned wrong value");
//}
//
//NOYX_TEST(IsMemberObjectPointer, UnitTest) {
//  tt_is_member_object_pointer_test<int A::*, true>();
//  tt_is_member_object_pointer_test<int A::*, true>();  // перевірка повторна, норм
//
//  tt_is_member_object_pointer_test<void (A::*)(), false>();
//  tt_is_member_object_pointer_test<int, false>();
//  tt_is_member_object_pointer_test<int*, false>();
//  tt_is_member_object_pointer_test<void (*)(), false>();
//}
