#include <type_traits/make_unsigned.hpp>
#include <type_traits/is_same.hpp>
#include <tt_test_detail.hpp>

template <typename T, typename Expected>
constexpr void tt_make_unsigned_test() {
  constexpr bool cond = xstl::is_same_v<xstl::make_unsigned_t<T>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(cond, "make_unsigned_t<T> returned wrong type");
#else
  NOYX_ASSERT_TRUE_MESSAGE(cond, "make_unsigned_t<T> returned wrong type: " << typeid(T).name());
#endif
}


NOYX_TEST(MakeUnsigned, UnitTest) {
  tt_make_unsigned_test<signed char, unsigned char>();
  tt_make_unsigned_test<short, unsigned short>();
  tt_make_unsigned_test<int, unsigned int>();
  tt_make_unsigned_test<long, unsigned int>();
  tt_make_unsigned_test<long long, unsigned long long>();

  tt_make_unsigned_test<unsigned char, unsigned char>();
  tt_make_unsigned_test<unsigned short, unsigned short>();
  tt_make_unsigned_test<unsigned int, unsigned int>();
  tt_make_unsigned_test<unsigned long, unsigned int>();
  tt_make_unsigned_test<unsigned long long, unsigned long long>();
}