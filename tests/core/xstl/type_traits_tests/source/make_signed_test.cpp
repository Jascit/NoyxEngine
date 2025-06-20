#include <type_traits/make_signed.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, typename Expected>
constexpr void tt_make_signed_test() {
  constexpr bool cond = xstl::is_same_v<xstl::make_signed_t<T>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(cond, "make_signed_t<T> returned wrong type");
#else
  NOYX_ASSERT_TRUE_MESSAGE(cond, "make_signed_t<T> returned wrong type: " << typeid(T).name());
#endif
}

NOYX_TEST(MakeSigned, UnitTest) {
  tt_make_signed_test<unsigned char, signed char>();
  tt_make_signed_test<unsigned short, short>();
  tt_make_signed_test<unsigned int, int>();
  tt_make_signed_test<unsigned long, int>();
  tt_make_signed_test<unsigned long long, long long>();

  tt_make_signed_test<signed char, signed char>();
  tt_make_signed_test<short, short>();
  tt_make_signed_test<int, int>();
  tt_make_signed_test<long, int>();
  tt_make_signed_test<long long, long long>();

}
