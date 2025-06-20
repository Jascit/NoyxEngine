//#include <type_traits/is_standard_layout.hpp>  // тв≥й заголовок
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
////  ласи дл€ альтернативних тест≥в
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
//    // Plain struct Ч стандартний layout
//    tt_is_standard_layout_test_value<Plain>(true);
//
//    //  лас з нестандартним конструктором Ч це не порушуЇ standard layout
//    tt_is_standard_layout_test_value<WithCtor>(true);
//
//    // —татичн≥ зм≥нн≥ не впливають на layout
//    tt_is_standard_layout_test_value<WithStatic>(true);
//
//    // ѕубл≥чне успадкуванн€ з Plain Ч still standard layout
//    tt_is_standard_layout_test_value<WithBasePublic>(true);
//
//    // ѕриватне успадкуванн€ порушуЇ standard layout
//    tt_is_standard_layout_test_value<WithBasePrivate>(false);
//
//    // ¬≥ртуальне успадкуванн€ Ч порушуЇ standard layout
//    tt_is_standard_layout_test_value<WithVirtualBase>(false);
//  }
//};
//
//NOYX_TEST(IsStandardLayoutAlt, UnitTest) {
//  TestTypeInvokerIsStandardLayoutAlt{}();
//}
