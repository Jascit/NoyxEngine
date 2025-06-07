#pragma once
#include <tests_details.hpp>
#include <integral_constant.hpp>  // deine xstl::integral_constant
#include <is_same.hpp>            // xstl::is_same_v
#include <type_traits>
#include <typeinfo>
#include <iostream>

#define FLAG_TEST_WITH_MESSAGE 1

template<typename T, T Val>
constexpr void tt_test_integral_constant() {
#if FLAG_TEST_WITH_MESSAGE
  std::cout << "Testing integral_constant<" << typeid(T).name() << ", " << +Val << ">\n";
#endif

  using IC = xstl::integral_constant<T, Val>;

  NOYX_ASSERT_TRUE_MESSAGE(
    IC::value == Val,
    std::string("IC::value mismatch for integral_constant<") << typeid(T).name() << ", " << std::to_string(Val) << ">"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::value_type, T>)),
    "value_type mismatch for integral_constant<" << typeid(T).name() << ", " << std::to_string(Val) << ">"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::type, IC>)),
    std::string("type alias mismatch for integral_constant<") << typeid(T).name() << ", " << std::to_string(Val) << ">"
  );

  {
    T tmp = IC{}();
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      std::string("operator()() mismatch for integral_constant<") << typeid(T).name() << ", " << std::to_string(Val) << ">"
    );
  }

  {
    T tmp = IC{};
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      std::string("conversion operator mismatch for integral_constant<") << typeid(T).name() << ", " << std::to_string(Val) << ">"
    );
  }
}

template<typename T, T Val>
struct IntegralConstantInvoker {
  constexpr void operator()() const {
    tt_test_integral_constant<T, Val>();
  }
};

NOYX_TEST(IntegralConstant, UnitTest) {
  std::cout << "\n----------TT_TESTS_INTEGRAL_CONSTANT----------\n";

  // einfache Typ/Wert-Kombinationen
  tt_test_integral_constant<int, 0>();
  tt_test_integral_constant<int, 42>();
  tt_test_integral_constant<bool, true>();
  tt_test_integral_constant<bool, false>();
  tt_test_integral_constant<char, 'x'>();
}

#undef FLAG_TEST_WITH_MESSAGE
