//#include <type_traits/is_aggregate.hpp>
//#include <tt_test_detail.hpp>
//#include <type_traits.hpp>
//
//template<typename T>
//constexpr void tt_is_aggregate_test_value(bool expected) {
//  constexpr bool actual = xstl::is_aggregate<T>::value;
//
//  NOYX_ASSERT_TRUE_MESSAGE(
//    actual == expected,
//    "is_aggregate<T> returned incorrect value"
//  );
//}
//
//struct Aggregate1 {
//  int x;
//  double y;
//};
//
//struct Aggregate2 {
//  int x;
//  Aggregate2() = default;
//};
//
//struct NonAggregate1 {
//  int x;
//  NonAggregate1() {} 
//};
//
//struct NonAggregate2 {
//private:
//  int x;
//public:
//  int y;
//};
//
//struct NonAggregate3 {
//  virtual void foo() {}
//};
//
//struct TestTypeInvokerIsAggregate {
//  constexpr void operator()() const {
//    tt_is_aggregate_test_value<int>(true);
//    tt_is_aggregate_test_value<Aggregate1>(true);
//    tt_is_aggregate_test_value<Aggregate2>(true);
//
//    tt_is_aggregate_test_value<NonAggregate1>(false);
//    tt_is_aggregate_test_value<NonAggregate2>(false);
//    tt_is_aggregate_test_value<NonAggregate3>(false);
//  }
//};
//
//NOYX_TEST(IsAggregate, UnitTest) {
//  TestTypeInvokerIsAggregate{}();
//}
