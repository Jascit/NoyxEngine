#pragma once

#include <iostream>
#include <tuple>
#include <tests_details.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_sufixes.hpp>
#include <tt_test.hpp>  // VALIDATE_TYPES(A,B)

namespace xstl_test_detail
{
    using all_test_types = std::tuple<
        bool, char, wchar_t, signed char, unsigned char,
        short, unsigned short, int, unsigned int,
        long, unsigned long, float, double, long double
    >;

    template <typename Tuple, template<typename> typename Fn, std::size_t... Is>
    constexpr void for_each_type_impl(std::index_sequence<Is...>) {
      (Fn<std::tuple_element_t<Is, Tuple>>{}(), ...);
    }

    template <typename Tuple, template<typename> typename Fn>
    constexpr void for_each_type() {
      xstl_test_detail::for_each_type_impl<Tuple, Fn>(
        std::make_index_sequence<std::tuple_size_v<Tuple>>{}
      );
    }

    template<std::size_t I = 0, template<typename> typename Invoker, typename Tuple>
    void test_all_types_with_suffix(Tuple&& tuple) {
      if constexpr (I < std::tuple_size_v<std::remove_reference_t<Tuple>>) {
        using T = std::tuple_element_t<I, std::remove_reference_t<Tuple>>;
        xstl_test_detail::for_each_type<xstl_test_detail::all_suffixes<T>, Invoker>();
        test_all_types_with_suffix<I + 1, Invoker>(std::forward<Tuple>(tuple));
      }
    }
} // namespace xstl_test_detail

#define TEST_WITH_STATIC_ASSERT 0

#if TEST_WITH_STATIC_ASSERT
#undef NOYX_FAIL
#define NOYX_FAIL() static_assert(false, "NOYX_FAIL triggered")
#undef NOYX_FAIL_MESSAGE
#define NOYX_FAIL_MESSAGE(message) static_assert(false, message)
#undef NOYX_ASSERT_TRUE
#define NOYX_ASSERT_TRUE(expr) static_assert(expr, "Assertion failed: " #expr)
#undef NOYX_ASSERT_TRUE_MESSAGE
#define NOYX_ASSERT_TRUE_MESSAGE(expr, message) static_assert(expr, message)
#undef NOYX_ASSERT_FALSE
#define NOYX_ASSERT_FALSE(expr) static_assert(!(expr), "Assertion failed: expected false: " #expr)
#undef NOYX_ASSERT_FALSE_MESSAGE
#define NOYX_ASSERT_FALSE_MESSAGE(expr, message) static_assert(!(expr), message)
#undef NOYX_ASSERT_EQ
#define NOYX_ASSERT_EQ(expected, actual) static_assert((expected) == (actual), "Assertion failed: expected == actual")
#undef NOYX_ASSERT_LT
#define NOYX_ASSERT_LT(a, b) static_assert((a) < (b), "Assertion failed: expected " #a " < " #b)
#undef NOYX_ASSERT_LE
#define NOYX_ASSERT_LE(a, b) static_assert((a) <= (b), "Assertion failed: expected " #a " <= " #b)
#undef NOYX_ASSERT_GT
#define NOYX_ASSERT_GT(a, b) static_assert((a) > (b), "Assertion failed: expected " #a " > " #b)
#undef NOYX_ASSERT_GE
#define NOYX_ASSERT_GE(a, b) static_assert((a) >= (b), "Assertion failed: expected " #a " >= " #b)
#undef NOYX_ASSERT_STREQ
#define NOYX_ASSERT_STREQ(a, b) static_assert(std::is_same_v<decltype(a), const char*> && std::is_same_v<decltype(b), const char*>, "NOYX_ASSERT_STREQ only works with const char*"); \
                                 static_assert(std::char_traits<char>::compare((a), (b), std::char_traits<char>::length(a)) == 0, "Assertion failed: expected equal C-strings")
#endif // TEST_WITH_STATIC_ASSERT
