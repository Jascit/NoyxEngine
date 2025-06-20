#include <type_traits/is_object.hpp>
#include <tt_test_detail.hpp>


template<typename T, bool Expected>
constexpr void tt_is_object_test_value() {
  constexpr bool actual1 = xstl::is_object<T>::value;
  constexpr bool actual2 = xstl::is_object_v<T>;

#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_object<" #T "> returned incorrect value"
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_object_v<" #T "> returned incorrect value"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual1 == Expected,
    "is_object<" << typeid(T).name()
    << "> actual = " << actual1 << ", expected = " << Expected
  );
  NOYX_ASSERT_TRUE_MESSAGE(
    actual2 == Expected,
    "is_object_v<" << typeid(T).name()
    << "> actual = " << actual2 << ", expected = " << Expected
  );
#endif
}

struct TestTypeInvokerIsObject {
  constexpr void operator()() const {
    tt_is_object_test_value<int, true>();
    tt_is_object_test_value<const double, true>();
    tt_is_object_test_value<int*, true>();
    tt_is_object_test_value<char[5], true>();

    enum E { A, B };
    struct S { int x; };
    tt_is_object_test_value<E, true>();
    tt_is_object_test_value<S, true>();

    tt_is_object_test_value<int S::*, true>();

    struct A { float arr[3]; };
    tt_is_object_test_value<decltype(&A::arr), true>();

    tt_is_object_test_value<void, false>();

    tt_is_object_test_value<int&, false>();
    tt_is_object_test_value<const S&, false>();

    tt_is_object_test_value<void(), false>();
    tt_is_object_test_value<int(int), false>();

    tt_is_object_test_value<void(*)(int), true>();

    tt_is_object_test_value<volatile S, true>();
    tt_is_object_test_value<const int*, true>();
    tt_is_object_test_value<const void*, true>();
  }
};

NOYX_TEST(IsObject, UnitTest) {
  TestTypeInvokerIsObject{}();
}
