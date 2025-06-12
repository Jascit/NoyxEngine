#include <type_traits/is_enum.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits>
#include <iostream>
#include <tt_test_detail.hpp>

enum UnscopedEnum { UE_A, UE_B };
enum class ScopedEnum : unsigned { SE_C, SE_D };

template<typename T, bool Expected>
constexpr void tt_is_enum_test_type() {
  constexpr bool actual = xstl::is_enum_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "xstl::is_enum_v error, actual !== Expected"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "xstl::is_enum_v<"
    << (Expected ? "enum" : "non-enum")
    << "> returned " << actual
    << ", expected " << Expected
  );
#endif // TEST_WITH_STATIC_ASSERT

}

template<typename T>
struct TestInvokerIsEnum {
  constexpr void operator()() const {
    tt_is_enum_test_type<T, false>();
  }
};

NOYX_TEST(IsEnum, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestInvokerIsEnum>();
  tt_is_enum_test_type<UnscopedEnum, true>();
  tt_is_enum_test_type<ScopedEnum, true>();
  tt_is_enum_test_type<const UnscopedEnum, true>();
  tt_is_enum_test_type<const ScopedEnum, true>();
  tt_is_enum_test_type<UnscopedEnum&, false>();
  tt_is_enum_test_type<ScopedEnum&&, false>();
}