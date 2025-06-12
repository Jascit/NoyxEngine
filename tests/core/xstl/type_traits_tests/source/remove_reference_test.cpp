#include <type_traits>
#include <tt_test_detail.hpp>
#include <type_traits/remove_reference.hpp>

template<typename T, typename Expected>
constexpr void test_remove_reference_single() {
    using RR = xstl::remove_reference<T>;
    using RR_t = xstl::remove_reference_t<T>;
#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<typename RR::type, Expected>::value)), "xstl::remove_reference<T>::type failed");
    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<RR_t, Expected>::value)), "xstl::remove_reference_t<T>::type failed");
#else
    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<typename RR::type, Expected>::value)), "xstl::remove_reference<T>::type failed for type: " << typeid(T).name());
    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<RR_t, Expected>::value)),
      "xstl::remove_reference_t<T> failed for type: " << typeid(T).name());
#endif
}

template<typename T>
class RemoveReferenceTester {
public:
    constexpr void operator()() const {
        test_remove_reference_single<T, T>();
        test_remove_reference_single<T&, T>();
        test_remove_reference_single<T&&, T>();
    }
};

NOYX_TEST(RemoveReference, UnitTest) {
    xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, RemoveReferenceTester>();
}
