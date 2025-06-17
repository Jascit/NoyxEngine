#include <type_traits/is_floating_point.hpp>
#include <tt_test_detail.hpp>

template<typename T, bool Exp>
void tt_is_floating_point_test() {
  NOYX_ASSERT_TRUE_MESSAGE(xstl::is_floating_point_v<T> == Exp, "tt_is_floating_point_test failed");
}

NOYX_TEST(FloatingPoint, UnitTest) {
  tt_is_floating_point_test<double, true>();
  tt_is_floating_point_test<float, true>();
  tt_is_floating_point_test<long double, true>();

  tt_is_floating_point_test<int, false>();
  tt_is_floating_point_test<char, false>();
  tt_is_floating_point_test<std::string, false>();
  tt_is_floating_point_test<void(), false>();
  tt_is_floating_point_test<void(&&)(), false>();
}