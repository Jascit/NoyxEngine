#include <type_traits>
#include <tests_details.hpp>
#include <tt_test_detail.hpp>
#include <remove_reference.hpp>

#define FLAG_TEST_WITH_MESSAGE 0

template<typename T, typename Expected>
constexpr void test_remove_reference_single() {
    using RR = xstl::remove_reference<T>;
    using RR_t = xstl::remove_reference_t<T>;

    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<typename RR::type, Expected>::value)), "xstl::remove_reference<T>::type failed for type: " << typeid(T).name());
    NOYX_ASSERT_TRUE_MESSAGE(NOYX_EVAL((xstl::is_same<RR_t, Expected>::value)),
      "xstl::remove_reference_t<T> failed for type: " << typeid(T).name());

#if FLAG_TEST_WITH_MESSAGE
    std::cout << "Tested remove_reference on: " << typeid(T).name() << "\n";
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
    std::cout << "\n----------TT_TESTS_REMOVE_REFERENCE----------\n";

    xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, RemoveReferenceTester>();
}