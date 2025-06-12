#include <type_traits/integral_constant.hpp>
#include <type_traits/conditional.hpp>
#include <type_traits/conjunction.hpp>
#include <tt_test_detail.hpp>    
#include <iostream>

template <bool... Bs>
struct BoolSeq {};

template <typename Seq>
struct TestInvokerConjunction;

template <bool... Bs>
struct TestInvokerConjunction<BoolSeq<Bs...>> {
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

using AllBoolSeqs = std::tuple<
  BoolSeq<>,                    // 0 Parameter
  BoolSeq<true>,                // 1 Parameter
  BoolSeq<false>,
  BoolSeq<true, true>,           // 2 Parameter
  BoolSeq<true, false>,
  BoolSeq<false, true>,
  BoolSeq<false, false>,
  BoolSeq<true, true, true>,      // 3 Parameter
  BoolSeq<true, true, false>,
  BoolSeq<true, false, true>,
  BoolSeq<false, true, true>,
  BoolSeq<true, false, false>,
  BoolSeq<false, true, false>,
  BoolSeq<false, false, true>,
  BoolSeq<false, false, false>
>;

template <typename Seq>
struct TestTypeInvokerConjunction {
  constexpr void operator()() const {
    TestInvokerConjunction<Seq>{}();
  }
};

NOYX_TEST(Conjunction, UnitTest) {
  xstl_test_detail::for_each_type<AllBoolSeqs, TestTypeInvokerConjunction>();
}