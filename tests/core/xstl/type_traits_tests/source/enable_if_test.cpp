#include <type_traits/enable_if.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T>
xstl::enable_if_t<xstl::is_same_v<T, int>, const char*> enable_if_overloaded() {
  return "int";
}

template <typename T>
xstl::enable_if_t<xstl::is_same_v<T, double>, const char*> enable_if_overloaded() {
  return "double";
}

constexpr void tt_enable_if_overload_test() {
  static_assert(xstl::is_same_v<decltype(enable_if_overloaded<int>()), const char*>, "overloaded<int> should return const char*");
  static_assert(xstl::is_same_v<decltype(enable_if_overloaded<double>()), const char*>, "overloaded<double> should return const char*");

  NOYX_ASSERT_TRUE_MESSAGE(enable_if_overloaded<int>() == std::string("int"), "overloaded<int>() returned wrong result");
  NOYX_ASSERT_TRUE_MESSAGE(enable_if_overloaded<double>() == std::string("double"), "overloaded<double>() returned wrong result");
}
