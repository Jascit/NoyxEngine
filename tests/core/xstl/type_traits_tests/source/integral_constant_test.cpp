#include <tt_test_detail.hpp>
#include <type_traits/integral_constant.hpp>  //  xstl::integral_constant
#include <type_traits/is_same.hpp>         // xstl::is_same_v
#include <type_traits>
#include <typeinfo>
#include <iostream>

template<typename T, T Val>
constexpr void tt_test_integral_constant() {

  using IC = xstl::integral_constant<T, Val>;
#if TEST_WITH_STATIC_ASSERT

  NOYX_ASSERT_TRUE_MESSAGE(
    IC::value == Val,
    "IC::value mismatch"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::value_type, T>)),
    "value_type mismatch"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::type, IC>)),
    "type alias mismatch"
  );

  {
    constexpr T tmp = IC{}();
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      "operator()() mismatch"
    );
  }

  {
    constexpr T tmp = IC{};
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      "conversion operator mismatch"
    );
  }

#else

  NOYX_ASSERT_TRUE_MESSAGE(
    IC::value == Val,
    std::string("IC::value mismatch for integral_constant<") + typeid(T).name() + ", " + std::to_string(Val) + ">"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::value_type, T>)),
    std::string("value_type mismatch for integral_constant<") + typeid(T).name() + ", " + std::to_string(Val) + ">"
  );

  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((xstl::is_same_v<typename IC::type, IC>)),
    std::string("type alias mismatch for integral_constant<") + typeid(T).name() + ", " + std::to_string(Val) + ">"
  );

  {
    T tmp = IC{}();
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      std::string("operator()() mismatch for integral_constant<") + typeid(T).name() + ", " + std::to_string(Val) + ">"
    );
  }

  {
    T tmp = IC{};
    NOYX_ASSERT_TRUE_MESSAGE(
      tmp == Val,
      std::string("conversion operator mismatch for integral_constant<") + typeid(T).name() + ", " + std::to_string(Val) + ">"
    );
  }

#endif
}

NOYX_TEST(IntegralConstant, UnitTest) {
  tt_test_integral_constant<int, 0>();
  tt_test_integral_constant<int, 42>();
  tt_test_integral_constant<bool, true>();
  tt_test_integral_constant<bool, false>();
  tt_test_integral_constant<char, 'x'>();
}

