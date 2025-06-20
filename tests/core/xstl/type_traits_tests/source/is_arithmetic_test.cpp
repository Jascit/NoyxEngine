//#include <type_traits/is_arithmetic.hpp>
//#include <type_traits/is_same.hpp>
//#include <tt_test_detail.hpp>
//
//template <typename T, bool Expected>
//constexpr void tt_is_arithmetic_test() {
//  static_assert(xstl::is_arithmetic<T>::value == Expected, "is_arithmetic returned wrong value");
//  static_assert(xstl::is_arithmetic_v<T> == Expected, "is_arithmetic_v returned wrong value");
//  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_arithmetic<T>::value == Expected, "is_arithmetic returned wrong value");
//}
//
//NOYX_TEST(IsArithmetic, UnitTest) {
//  // Цілі типи
//  tt_is_arithmetic_test<int, true>();
//  tt_is_arithmetic_test<unsigned int, true>();
//  tt_is_arithmetic_test<long, true>();
//  tt_is_arithmetic_test<unsigned long, true>();
//  tt_is_arithmetic_test<signed char, true>();
//  tt_is_arithmetic_test<unsigned char, true>();
//
//  // Плаваючі
//  tt_is_arithmetic_test<float, true>();
//  tt_is_arithmetic_test<double, true>();
//  tt_is_arithmetic_test<long double, true>();
//
//  // Не арифметичні
//  tt_is_arithmetic_test<bool, false>(); // bool часто не вважають арифметичним, але за стандартом це інтегральний
//  tt_is_arithmetic_test<char, false>(); // залежить від реалізації, можна підкоригувати
//  tt_is_arithmetic_test<void, false>();
//  tt_is_arithmetic_test<int*, false>();
//  tt_is_arithmetic_test<std::nullptr_t, false>();
//}
