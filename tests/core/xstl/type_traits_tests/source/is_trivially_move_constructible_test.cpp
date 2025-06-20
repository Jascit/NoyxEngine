//#include <type_traits/is_trivially_move_constructible.hpp>
//#include <tt_test_detail.hpp>
//#include <type_traits.hpp>
//
//template<typename T>
//constexpr void tt_is_trivially_move_constructible_test_value(bool expected) {
//  constexpr bool actual = xstl::is_trivially_move_constructible<T>::value;
//
//  NOYX_ASSERT_TRUE_MESSAGE(
//    actual == expected,
//    "is_trivially_move_constructible<T> returned incorrect value"
//  );
//}
//
//struct TrivialMoveConstructible {
//  int x;
//  double y;
//};
//
//struct NonTrivialMoveConstructible {
//  NonTrivialMoveConstructible(NonTrivialMoveConstructible&&) {
//  }
//};
//
//struct DeletedMoveConstructible {
//  DeletedMoveConstructible(DeletedMoveConstructible&&) = delete;
//};
//
//struct WithVirtual {
//  virtual void foo() {}
//};
//
//struct TestTypeInvokerIsTriviallyMoveConstructible {
//  constexpr void operator()() const {
//    tt_is_trivially_move_constructible_test_value<int>(true);
//    tt_is_trivially_move_constructible_test_value<double>(true);
//    tt_is_trivially_move_constructible_test_value<TrivialMoveConstructible>(true);
//
//    tt_is_trivially_move_constructible_test_value<NonTrivialMoveConstructible>(false);
//    tt_is_trivially_move_constructible_test_value<DeletedMoveConstructible>(false);
//    tt_is_trivially_move_constructible_test_value<WithVirtual>(false);
//
//    tt_is_trivially_move_constructible_test_value<int[3]>(true);
//  }
//};
//
//NOYX_TEST(IsTriviallyMoveConstructible, UnitTest) {
//  TestTypeInvokerIsTriviallyMoveConstructible{}();
//}
