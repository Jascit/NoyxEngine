#include <type_traits/integral_constant.hpp>
#include <type_traits/conditional.hpp>
#include <type_traits/conjunction.hpp>
#include <tt_test_detail.hpp>    
#include <iostream>

template <bool... Bs>
struct Conjunction_BoolSeq {};

template <typename Seq>
struct TestInvokerConjunction;

template <bool... Bs>
struct TestInvokerConjunction<Conjunction_BoolSeq<Bs...>> {
  constexpr void operator()() const {
    using C = xstl::conjunction<xstl::integral_constant<bool, Bs>...>;
    constexpr bool expected = (Bs && ...);  // C++17-Fold

    if constexpr (expected) {
#if TEST_WITH_STATIC_ASSERT
      NOYX_ASSERT_TRUE_MESSAGE(
        C::value,
        "xstl::conjunction muss true liefern"
      );
    }
    else {
      NOYX_ASSERT_FALSE_MESSAGE(
        C::value,
        "xstl::conjunction muss false liefern"
      );
#else
      //std::cout << "C type: " << typeid(C::type).name() << "\n";
      NOYX_ASSERT_TRUE_MESSAGE(
        C::value,
        "xstl::conjunction<Bs...> liefert false, erwartet true"
      );
    }
    else {
      //std::cout << "C type: " << typeid(C::type).name() << "\n";
      NOYX_ASSERT_FALSE_MESSAGE(
        C::value,
        "xstl::conjunction<Bs...> liefert true, erwartet false"
      );
#endif
    }
  }
};

template <typename Seq>
struct TestTypeInvokerConjunction {
  constexpr void operator()() const {
    TestInvokerConjunction<Seq>{}();
  }
};

NOYX_TEST(Conjunction, UnitTest) {
  using AllBoolSeqs = std::tuple<
    Conjunction_BoolSeq<>,                    // 0 Parameter
    Conjunction_BoolSeq<true>,                // 1 Parameter
    Conjunction_BoolSeq<false>,
    Conjunction_BoolSeq<true, true>,           // 2 Parameter
    Conjunction_BoolSeq<true, false>,
    Conjunction_BoolSeq<false, true>,
    Conjunction_BoolSeq<false, false>,
    Conjunction_BoolSeq<true, true, true>,      // 3 Parameter
    Conjunction_BoolSeq<true, true, false>,
    Conjunction_BoolSeq<true, false, true>,
    Conjunction_BoolSeq<false, true, true>,
    Conjunction_BoolSeq<true, false, false>,
    Conjunction_BoolSeq<false, true, false>,
    Conjunction_BoolSeq<false, false, true>,
    Conjunction_BoolSeq<false, false, false>
  >;
  xstl_test_detail::for_each_type<AllBoolSeqs, TestTypeInvokerConjunction>();
}