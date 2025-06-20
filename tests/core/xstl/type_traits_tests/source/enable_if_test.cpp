#include <type_traits/enable_if.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

using xstl::enable_if_t;
using xstl::is_same_v;

template <typename T>
enable_if_t<is_same_v<T, int>, const char*> overloaded() {
  return "int";
}

template <typename T>
enable_if_t<is_same_v<T, double>, const char*> overloaded() {
  return "double";
}

constexpr void tt_enable_if_overload_test() {
  static_assert(is_same_v<decltype(overloaded<int>()), const char*>, "overloaded<int> should return const char*");
  static_assert(is_same_v<decltype(overloaded<double>()), const char*>, "overloaded<double> should return const char*");

  NOYX_ASSERT_TRUE_MESSAGE(overloaded<int>() == std::string("int"), "overloaded<int>() returned wrong result");
  NOYX_ASSERT_TRUE_MESSAGE(overloaded<double>() == std::string("double"), "overloaded<double>() returned wrong result");
}
