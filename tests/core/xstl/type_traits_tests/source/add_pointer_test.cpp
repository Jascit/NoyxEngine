#include <tt_test_detail.hpp>
#include <type_traits/add_pointer.hpp>
#include <type_traits/is_same.hpp>

template<typename T, typename Expected>
constexpr void tt_add_pointer_test_value() {
  constexpr bool actual = xstl::is_same_v<xstl::add_pointer_t<T>, Expected>;
#if TEST_WITH_STATIC_ASSERT
  NOYX_ASSERT_TRUE_MESSAGE(
    actual,
    "add_pointer<" #T "> returned incorrect type"
  );
#else
  NOYX_ASSERT_TRUE_MESSAGE(
    actual == true,
    "add_pointer<" << typeid(T).name() << "> returned incorrect: "
    "actual = " << typeid(xstl::add_pointer_t<T>).name()
    << ", expected = " << typeid(Expected).name()
  );
#endif
}

struct TestTypeInvokerAddPointer {
  void operator()() const {
    tt_add_pointer_test_value<int, int*>();
    tt_add_pointer_test_value<int&, int*>();
    tt_add_pointer_test_value<int&&, int*>();
    tt_add_pointer_test_value<const int&, const int*>();
    tt_add_pointer_test_value<int*, int**>();
    tt_add_pointer_test_value<int**, int***>();
    tt_add_pointer_test_value<int[5], int(*)[5]>();
    tt_add_pointer_test_value<int[], int(*)[]>();
    tt_add_pointer_test_value<void(), void(*)()>();
    tt_add_pointer_test_value<void(*)(), void(**)()>();
  }
};

NOYX_TEST(AddPointer, UnitTest) {
  TestTypeInvokerAddPointer{}();
}
