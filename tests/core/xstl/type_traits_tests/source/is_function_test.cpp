#include <type_traits/is_function.hpp>
#include <type_traits/remove_all_extents.hpp>
#include <type_traits/conditional.hpp>
#include <iostream>
#include <tt_test_detail.hpp>

template<typename T, bool Expected>
constexpr void tt_is_function_test_type() {
  constexpr bool actual = xstl::is_function_v<T>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_function<T> returned incorrect result"
  );
#else
  //std::cout << "is_function<T> returned incorrect result: got " << actual << ", expected " << Expected << "\n" << typeid(T).name() << "\n";
  NOYX_ASSERT_TRUE_MESSAGE(
    NOYX_EVAL((actual == Expected)),
    "is_function<T> returned incorrect result: got " << actual << ", expected " << Expected << "\n"
    << typeid(T).name() << "\n"
  );
#endif
}

template<typename T>
struct TestTypeInvokerIsFunction {
  constexpr void operator()() const {
    using Tr = xstl::remove_reference_t<T>;
    using Tre = xstl::remove_all_extents<Tr>;
    tt_is_function_test_type<Tre, false>();
    tt_is_function_test_type<Tre*, false>();
    tt_is_function_test_type<Tre&, false>();
    tt_is_function_test_type<Tre&&, false>();
    tt_is_function_test_type<const Tre, false>();
    tt_is_function_test_type<volatile Tre, false>();
    tt_is_function_test_type<const volatile Tre, false>();

    tt_is_function_test_type<void(), true>();
    tt_is_function_test_type<void(Tre), true>();
    tt_is_function_test_type<int(Tre, double), true>();
    tt_is_function_test_type<void(...) noexcept, true>();
    tt_is_function_test_type<int(Tre, ...) noexcept, true>();

    tt_is_function_test_type<void(*)(), false>();
    tt_is_function_test_type<int(*)(Tre), false>();
    tt_is_function_test_type<void(&)(Tre), false>();
    tt_is_function_test_type<void(&&)(Tre), false>();
  }
};

NOYX_TEST(IsFunction, UnitTest) {
  xstl_test_detail::for_each_type<xstl_test_detail::all_test_types, TestTypeInvokerIsFunction>();
  xstl_test_detail::test_all_types_with_suffix<0, TestTypeInvokerIsFunction, xstl_test_detail::all_test_types>(xstl_test_detail::all_test_types{});
}
