//#include <type_traits/is_standard_layout.hpp>  // ��� ���������
//#include <tt_test_detail.hpp>
//#include <type_traits.hpp>
//
//template<typename T>
//constexpr void tt_is_standard_layout_test_value(bool expected) {
//  constexpr bool actual = xstl::is_standard_layout<T>::value;
//
//  NOYX_ASSERT_TRUE_MESSAGE(
//    actual == expected,
//    "is_standard_layout<T> returned incorrect value"
//  );
//}
//
//// ����� ��� �������������� �����
//struct Plain {
//  int a;
//  char b;
//};
//
//struct WithCtor {
//  WithCtor() {}
//  int x;
//};
//
//struct WithStatic {
//  static int s;
//  int x;
//};
//
//struct WithBasePublic : Plain {};
//
//struct WithBasePrivate : private Plain {};
//
//struct WithVirtualBase : virtual Plain {};
//
//struct TestTypeInvokerIsStandardLayoutAlt {
//  constexpr void operator()() const {
//    // Plain struct � ����������� layout
//    tt_is_standard_layout_test_value<Plain>(true);
//
//    // ���� � ������������� ������������� � �� �� ������ standard layout
//    tt_is_standard_layout_test_value<WithCtor>(true);
//
//    // ������� ���� �� ��������� �� layout
//    tt_is_standard_layout_test_value<WithStatic>(true);
//
//    // ������� ������������ � Plain � still standard layout
//    tt_is_standard_layout_test_value<WithBasePublic>(true);
//
//    // �������� ������������ ������ standard layout
//    tt_is_standard_layout_test_value<WithBasePrivate>(false);
//
//    // ³�������� ������������ � ������ standard layout
//    tt_is_standard_layout_test_value<WithVirtualBase>(false);
//  }
//};
//
//NOYX_TEST(IsStandardLayoutAlt, UnitTest) {
//  TestTypeInvokerIsStandardLayoutAlt{}();
//}
