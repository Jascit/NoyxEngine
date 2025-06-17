#include <type_traits/remove_cvref_t.hpp>
#include <type_traits/is_same.hpp>
#include <type_traits/is_array.hpp>
#include <type_traits/is_function.hpp>
#include <type_traits/add_pointer.hpp>
#include <type_traits/remove_all_extents.hpp>
#include <tt_test_detail.hpp>
#include <type_traits/decay.hpp>
#include <iostream>

template<typename T, typename Expected>
constexpr void tt_decay_test_type() {
    using actual = xstl::decay_t<T>;

#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<actual, Expected>)),
        "decay<T> returned incorrect type"
    );
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((xstl::is_same_v<actual, Expected>)),
        "decay<T> returned incorrect type: got different from expected"
    );
#endif
}

template<typename T>
struct TestTypeInvokerDecay {
    constexpr void operator()() const {
        using Tr = xstl::remove_reference_t<T>;
        using Tre = xstl::remove_all_extents_t<Tr>;
        using Tfunc = void(int);

        // Базові перевірки decay
        tt_decay_test_type<T, T>(); // без модифікацій (можеш адаптувати під конкретні типи)

        // Конкретні випадки
        tt_decay_test_type<int, int>();
        tt_decay_test_type<const int, int>();
        tt_decay_test_type<int&, int>();
        tt_decay_test_type<int&&, int>();
        tt_decay_test_type<int[3], int*>();
        tt_decay_test_type<const int[5], const int*>();
        tt_decay_test_type<Tfunc, void(*)(int)>();
        tt_decay_test_type<void(int), void(*)(int)>();
        tt_decay_test_type<int(int), int(*)(int)>();
    }
};

NOYX_TEST(Decay, UnitTest) {
    xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerDecay>();
}
