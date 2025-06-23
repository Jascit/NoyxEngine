#pragma once
#include <type_traits/swap.hpp>                            
#include <type_traits/is_move_constructible.hpp>
#include <type_traits/is_move_assignable.hpp>
#include <tt_test_detail.hpp>                  

template<typename T, bool Expected>
constexpr void tt_is_swappable_test_type()
{
    constexpr bool actual = xstl::details::_ConceptIsSwapable<T>;
#if TEST_WITH_STATIC_ASSERT
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((actual == Expected)),
        "ConceptIsSwapable  FAILED"
    );
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((actual == Expected)),
        "ConceptIsSwapable<" << typeid(T).name()
        << "> returned " << actual << " but expected " << Expected
    );
#endif
}

template<typename T>
struct TestTypeInvokerIsSwappable
{
    constexpr void operator()() const
    {
        tt_is_swappable_test_type<T, true>(); 
        tt_is_swappable_test_type<const T, false>(); 
        tt_is_swappable_test_type<T&, true>(); 
        tt_is_swappable_test_type<T&&, true>();
        tt_is_swappable_test_type<T[3], false>();
        tt_is_swappable_test_type<T*, true>();
    }
};

NOYX_TEST(ConceptIsSwapable, UnitTest)
{
    xstl_test_detail::for_each_type<std::tuple<int, long, char, double>, TestTypeInvokerIsSwappable>();
}



template<typename T>
void tt_swap_runtime_test()
{
    T a{ 1 }, b{ 2 };
    std::swap(a, b);
#if TEST_WITH_STATIC_ASSERT
#else
    NOYX_ASSERT_TRUE_MESSAGE(
        NOYX_EVAL((a.value == 2 && b.value == 1)),
        "xstl::swap failed for type " << typeid(T).name()
    );
#endif
}

NOYX_TEST(FunctionSwap, UnitTest)
{
  struct MovableOnly
  {
    int value;
    MovableOnly(int v) : value(v) {}
    MovableOnly(MovableOnly&& o)  noexcept : value(o.value) {}
    MovableOnly& operator=(MovableOnly&& o) noexcept { value = o.value; return *this; }

    MovableOnly(const MovableOnly&) = delete;
    MovableOnly& operator=(const MovableOnly&) = delete;
  };
  struct WrapInt { int value; WrapInt(int v) :value(v) {} WrapInt(WrapInt&&) = default; WrapInt& operator=(WrapInt&&) = default; };
  tt_swap_runtime_test<WrapInt>();

  tt_swap_runtime_test<MovableOnly>();
}
