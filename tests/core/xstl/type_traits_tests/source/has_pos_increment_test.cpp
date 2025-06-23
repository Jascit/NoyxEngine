#include <tt_test_detail.hpp>
#include <type_traits/has_post_increment.hpp>

template<typename T, bool Expected>
constexpr void tt_has_post_increment_test_value() {
  constexpr bool actual = xstl::has_post_increment<T>::value;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_post_increment<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == Expected,
    "has_post_increment<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << actual << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerHasPostIncrement {
  struct U1 {
    U1 operator++(int);
    U1(const U1&) = delete;
    U1(U1&&) = delete;
  };
  struct U2 {
  private:
    U2 operator++(int);
  };

  void operator()() const {
    tt_has_post_increment_test_value<int, true>();
    tt_has_post_increment_test_value<int&, true>();
    tt_has_post_increment_test_value<const int&, false>();
    tt_has_post_increment_test_value<int*, true>();
    tt_has_post_increment_test_value<int*&, true>();

    tt_has_post_increment_test_value<U1, true>();
    tt_has_post_increment_test_value<U1&, true>();
    tt_has_post_increment_test_value<const U1&, false>();
    tt_has_post_increment_test_value<U2&, false>();
  }
};

NOYX_TEST(HasPostIncrement, UnitTest) {
  TestTypeInvokerHasPostIncrement{}();
}
