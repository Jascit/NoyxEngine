#include <type_traits/integer_sequence.hpp>
#include <type_traits/index_sequence.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template<typename T, T N, T... Expected>
constexpr void tt_make_integer_sequence_test() {
    using Actual = xstl::make_integer_sequence<T, N>;
    using ExpectedS = xstl::integer_sequence<T, Expected...>;

#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_integer_sequence<T,N> returned wrong sequence"
    );
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_integer_sequence<" << N << "> returned wrong sequence"
    );
#endif
}

template<std::size_t N, std::size_t... Expected>
constexpr void tt_make_index_sequence_test() {
    using Actual = xstl::make_index_sequence<N>;
    using ExpectedS = xstl::integer_sequence<xstl::size_t, Expected...>;

#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_index_sequence<N> returned wrong sequence"
    );
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_index_sequence<" << N << "> returned wrong sequence"
    );
#endif
}

template<typename... Args, std::size_t... Expected>
constexpr void tt_make_sequence_for_test(std::index_sequence<Expected...>) {
    using Actual = xstl::make_sequence_for<Args...>;
    using ExpectedS = xstl::integer_sequence<xstl::size_t, Expected...>;

#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_sequence_for<Args...> returned wrong sequence"
    );
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<Actual, ExpectedS>)),
        "make_sequence_for<" << sizeof...(Args) << "> returned wrong sequence"
    );
#endif
}

NOYX_TEST(IntegerSequence, UnitTest) {
    // make_integer_sequence<T,N>
    tt_make_integer_sequence_test<int, 0>();                     // []
    tt_make_integer_sequence_test<int, 1, 0>();                  // [0]
    tt_make_integer_sequence_test<int, 5, 0, 1, 2, 3, 4>();       // [0,1,2,3,4]

    // make_index_sequence<N>
    tt_make_index_sequence_test<0>();                            // []
    tt_make_index_sequence_test<1, 0>();                         // [0]
    tt_make_index_sequence_test<4, 0, 1, 2, 3>();                // [0,1,2,3]

    // make_sequence_for<Args...>
    tt_make_sequence_for_test<char>(std::make_index_sequence<1>{});                   // [0]
    tt_make_sequence_for_test<int, double>(std::make_index_sequence<2>{});            // [0,1]
    tt_make_sequence_for_test<float, long, long long>(std::make_index_sequence<3>{}); // [0,1,2]
}
